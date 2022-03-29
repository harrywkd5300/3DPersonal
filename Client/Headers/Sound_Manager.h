#pragma once

#include "Defines.h"
#include "Base.h"
#include "engine_defines.h"

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	enum ChannelID { Channel_Player, Channel_Mon, Channel_Santa, Channel_Summon, Channel_FootWalk,Channel_BGM, Channel_TowerAtk0, Channel_TowerAtk1, Channel_End };

public:
	explicit CSound_Manager(void);
	virtual ~CSound_Manager(void) = default;

public:
	HRESULT Ready_Sound_Manager();
	void Play_Sound(TCHAR* pSoundKey, ChannelID eID, const _float& volume);
	void Play_BGM(TCHAR* pSoundKey, ChannelID eID, const _float& volume);
	void Update_Sound();
	void Stop_Sound(ChannelID eID);
	void Stop_All();

public:
	HRESULT LoadSoundFile();

private:
	FMOD_SYSTEM*	m_pSystem;
	FMOD_CHANNEL*	m_pChannelArr[Channel_End]; // 채널은 중첩되지 않게 준비.

	map<const TCHAR*, FMOD_SOUND*>	m_MapSound;

private:
	virtual _ulong Free();

};
