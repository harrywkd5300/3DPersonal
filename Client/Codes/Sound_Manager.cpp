#include "StdAfx.h"
#include "..\Headers\Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager(void)
{

}

HRESULT CSound_Manager::Ready_Sound_Manager()
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, Channel_End, FMOD_INIT_NORMAL, NULL);	

	if (FAILED(LoadSoundFile()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSound_Manager::LoadSoundFile()
{
	_finddata_t fd;
	int iResult = 0;
	long handle = _findfirst("../Bin/Sound/*.*", &fd);
	if (-1 == handle)
	{
		MessageBox(g_hWnd, L"Not found file!!", L"Not found file!!", MB_OK);
		return E_FAIL;
	}
	while (-1 != iResult)
	{
		char szFullPath[256] = "";
		strcpy_s(szFullPath, "../Bin/Sound/");
		strcat_s(szFullPath, fd.name);
		FMOD_SOUND* pSound;
		FMOD_RESULT FRes = FMOD_System_CreateSound(m_pSystem, szFullPath
			, FMOD_HARDWARE, NULL, &pSound);
		if (FMOD_OK == FRes)
		{
			TCHAR* pSoundKey = new TCHAR[256];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * 256);

			MultiByteToWideChar(CP_ACP, 0, fd.name, strlen(fd.name) + 1,
				pSoundKey, 256);

			m_MapSound.insert(make_pair(pSoundKey, pSound));
		}
		iResult = _findnext(handle, &fd);
	}
	_findclose(handle);
	FMOD_System_Update(m_pSystem);

	return NOERROR;
}

void CSound_Manager::Play_Sound(TCHAR* pSoundKey, ChannelID eID, const _float& volume)
{
	map<const TCHAR*, FMOD_SOUND*>::iterator iter = find_if(
		m_MapSound.begin(), m_MapSound.end(), CStringCmp(pSoundKey));
	if (iter == m_MapSound.end())
		return;
	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_OFF);
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, 0, &(m_pChannelArr[eID]));
	FMOD_Channel_SetVolume(m_pChannelArr[eID], volume);
}

void CSound_Manager::Play_BGM(TCHAR* pSoundKey, ChannelID eID, const _float& volume)
{
	map<const TCHAR*, FMOD_SOUND*>::iterator iter = find_if(
		m_MapSound.begin(), m_MapSound.end(), CStringCmp(pSoundKey));
	if (iter == m_MapSound.end())
		return;
	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, 0, &(m_pChannelArr[eID]));
	FMOD_Channel_SetVolume(m_pChannelArr[eID], volume);
}

void CSound_Manager::Update_Sound()
{
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Stop_Sound(ChannelID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::Stop_All()
{
	for (int i = 0; i < Channel_End; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

_ulong CSound_Manager::Free()
{
	_ulong dwRefCnt = 0;
	map<const TCHAR*, FMOD_SOUND*>::iterator iter_begin = m_MapSound.begin();
	map<const TCHAR*, FMOD_SOUND*>::iterator iter_end = m_MapSound.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		delete[] iter_begin->first;
		FMOD_Sound_Release(iter_begin->second);
	}

	m_MapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);

	return dwRefCnt;
}

