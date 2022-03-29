#include "stdafx.h"
#include "..\Headers\Scene_Boss.h"

#include "object_Manager.h"
#include "Sound_Manager.h"
#include "layer.h"
#include "Player.h"

#include "BossTerrain.h"
#include "Sky.h"
#include "Player.h"
#include "CrossBow.h"

#include "Camera_Static.h"

#include "TestNavi.h"
#include "BaseObj.h"
#include "NoCollObj.h"
#include "Boss.h"
#include "Crystal.h"
#include "Ending.h"

CScene_Boss::CScene_Boss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

HRESULT CScene_Boss::Ready_Scene(void)
{
	if (FAILED(Set_UI()))
		return E_FAIL;

	if (FAILED(Ready_LightInfo()))
		return E_FAIL;

	if (FAILED(Add_BGMForStageBoss()))
		return E_FAIL;

	if (FAILED(Ready_Load_Object()))
		return E_FAIL;

	if (FAILED(Change_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(L"Layer_Object")))
		return E_FAIL;

	if (FAILED(Set_Player()))
		return E_FAIL;

	if (FAILED(Set_Boss()))
		return E_FAIL;

	if (FAILED(Ready_Layer_TEST(L"Layer_Test")))
		return E_FAIL;

	if (FAILED(Ready_Crystal(L"Layer_Crystal")))
		return E_FAIL;

	return NOERROR;
}

_int CScene_Boss::Update_Scene(const _float & fTimeDelta)
{
	if (CWave_Manager::GetInstance()->Get_WaveList()->size() == 0)
	{
		CObject_Manager::GetInstance()->Release_TargetLayer(SCENE_STATIC, L"Layer_UI");

		CSound_Manager::GetInstance()->Stop_Sound(CSound_Manager::Channel_BGM);

		CGameObject* pObject = CEnding::Create(m_pGraphicDev);
		if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_BOSS, L"Ending", pObject)))
			return 1;
	}


	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Boss::Render_Scene(void)
{
	Engine::CScene::Render_Scene();
}

HRESULT CScene_Boss::Ready_LightInfo()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DLIGHT9		LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Ambient = LightInfo.Diffuse;
	LightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	m_pGraphicDev->SetLight(0, &LightInfo);
	m_pGraphicDev->LightEnable(0, TRUE);

	return NOERROR;
}

HRESULT CScene_Boss::Add_BGMForStageBoss()
{
	CSound_Manager::GetInstance()->Stop_Sound(CSound_Manager::Channel_BGM);
	CSound_Manager::GetInstance()->Play_BGM(L"BGM2.mp3", CSound_Manager::Channel_BGM, 0.8f);

	return NOERROR;
}

HRESULT CScene_Boss::Set_UI()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_UI")->GetObjList()->begin();
	auto& iter_end = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_UI")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_isUpdate(false);
	}

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Load_Object()
{
	// BossStage00
	HANDLE hFile = CreateFile(L"../Bin/Data/BossStage00.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);

	// BossStageTest
	//HANDLE hFile = CreateFile(L"../Bin/Data/Test8.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
	//	, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Load Failed!!");
		return E_FAIL;
	}

	DWORD dwByte = 0;

	_ulong dwNumObject = 0;

	ReadFile(hFile, &dwNumObject, sizeof(_ulong), &dwByte, nullptr);

	m_vecObject.reserve(dwNumObject);

	while (true)
	{
		OBJECT* pObject = new OBJECT;
		ZeroMemory(pObject, sizeof(OBJECT));

		ReadFile(hFile, pObject, sizeof(OBJECT), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Engine::Safe_Delete(pObject);
			break;
		}
		m_vecObject.push_back(pObject);
	}
	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Crystal(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Object
	pGameObject = CCrystal::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Layer_Object(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	_uint iSize = m_vecObject.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		Engine::CGameObject*	pGameObject = nullptr;
		if (!lstrcmp(m_vecObject[i]->FileName, L"NutCracker.X") || !lstrcmp(m_vecObject[i]->FileName, L"Barrel.X"))
		{
			// For.Object
			pGameObject = CBaseObj::Create(m_pGraphicDev, m_vecObject[i]->FullPath,
				m_vecObject[i]->FileName, m_vecObject[i]->ComName, m_vecObject[i]->vScale, m_vecObject[i]->vPostion, m_vecObject[i]->vRotation, SCENE_BOSS);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, pLayerTag, pGameObject)))
				return E_FAIL;
		}
		else
		{
			// For.NoColObj
			pGameObject = CNoCollObj::Create(m_pGraphicDev, m_vecObject[i]->FullPath,
				m_vecObject[i]->FileName, m_vecObject[i]->ComName, m_vecObject[i]->vScale, m_vecObject[i]->vPostion, m_vecObject[i]->vRotation, SCENE_BOSS);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, L"Layer_NoColObj", pGameObject)))
				return E_FAIL;
		}
	}

	return NOERROR;
}

HRESULT CScene_Boss::Change_Camera()
{
	m_pCamera_Manager->Change_Camera(L"Static_Camera");

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CBossTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.Sky
	pGameObject = CSky::Create(m_pGraphicDev, SCENE_BOSS);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, pLayerTag, pGameObject)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CScene_Boss::Set_Player()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();

	dynamic_cast<CPlayer*>(iter)->Set_UpdateBool(false);

	dynamic_cast<CPlayer*>(iter)->Set_CurType(SCENE_BOSS);

	//dynamic_cast<CPlayer*>(iter)->Set_IsColl(false);

	dynamic_cast<CPlayer*>(iter)->Release_Component();

	dynamic_cast<CPlayer*>(iter)->Set_Position(&_vec3(62.f, 0.f, 30.f));

	dynamic_cast<CPlayer*>(iter)->Set_Scale(&_vec3(0.05f, 0.05f, 0.05f));

	CCamera* pCamera = m_pCamera_Manager->Get_Camera(L"Static_Camera");

	dynamic_cast<CCamera_Static*>(pCamera)->Set_Target();

	return NOERROR;
}

HRESULT CScene_Boss::Set_Boss()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();

	dynamic_cast<CBoss*>(iter)->Set_MakeSnowMan(true);

	dynamic_cast<CBoss*>(iter)->Set_CurType(SCENE_BOSS);

	dynamic_cast<CBoss*>(iter)->Release_Component();

	dynamic_cast<CBoss*>(iter)->Set_Position(&_vec3(53.f, 1.f, 63.f));

	dynamic_cast<CBoss*>(iter)->Set_Scale(&_vec3(0.02f, 0.02f, 0.02f));

	CWave_Manager::GetInstance()->Add_WaveMonster(CWave_Manager::POINT_A, iter);

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Layer_TEST(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.TestNavi
	pGameObject = CTestNavi::Create(m_pGraphicDev, SCENE_BOSS);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_BOSS, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

CScene_Boss * CScene_Boss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Boss *	pInstance = new CScene_Boss(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("CScene_Boss Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CScene_Boss::Free(void)
{
	auto& iter = m_vecObject.begin();
	auto& iter_end = m_vecObject.end();
	for (; iter != iter_end; ++iter)
	{
		Safe_Delete((*iter));
	}
	m_vecObject.clear();

	if (FAILED(CScene::Release_Object(SCENE_BOSS)))
		MSG_BOX("Scene_Logo Release Failed");

	if (FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_BOSS)))
		MSG_BOX("Scene_Logo Release Failed");

	return Engine::CScene::Free();
}
