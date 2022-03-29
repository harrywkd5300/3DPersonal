#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"

#include "Terrain.h"
#include "Sky.h"
#include "Player.h"
#include "CrossBow.h"

//#include "TestObj.h"
#include "TestNavi.h"
#include "BaseObj.h"
#include "Tree.h"
#include "NoCollObj.h"
#include "Door.h"
#include "Boss.h"
#include "SnowmanBoss.h"
#include "Goblin.h"
#include "DarkElfArcher.h"
#include "Kobold.h"
#include "OrcBruiser.h"
#include "Santa.h"
#include "Snow.h"
#include "management.h"
#include "BossLogo.h"
#include "Component_manager.h"
#include "Camera_Dynamic.h"
#include "Camera_Static.h"
#include "Input_Device.h"
#include "Layer.h"

#include "Sound_Manager.h"
#include "CollisionMgr.h"

#include "DoorIn.h"
// UI
#include "Panel.h"
#include "SkillIcon.h"
#include "HpBar.h"
#include "MpBar.h"
#include "Aim.h"
#include "Phase.h"

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pWave_Manager(CWave_Manager::GetInstance())
	, m_pColl_Manager(CCollisionMgr::GetInstance())
{
	m_pWave_Manager->AddRef();
}

HRESULT CScene_Stage::Ready_Scene(void)
{
	if (FAILED(Ready_LightInfo()))
		return E_FAIL;

	if (FAILED(Add_BGM_ForStage()))
		return E_FAIL;

	if (FAILED(Ready_Load_Object()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(&_vec3(0.f, 0.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(L"Layer_Object")))
		return E_FAIL;

	if (FAILED(Ready_Layer_LeftDoor(L"Layer_LeftDoor")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Boss(L"Layer_Boss")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Snowman(L"Layer_SnowBoss")))
		return E_FAIL;

	if (FAILED(Ready_Layer_RightDoor(L"Layer_RightDoor")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Snow(L"Layer_SnowPaticle")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Santa(L"Layer_Santa")))
		return E_FAIL;

	if (FAILED(Ready_Layer_TEST(L"Layer_Test")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	return NOERROR;
}

_int CScene_Stage::Update_Scene(const _float & fTimeDelta)
{
	m_fTimeDeta = fTimeDelta;

#ifdef _DEBUG
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_P) & 0x80)
	{
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_O) & 0x80)
	{
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
#endif 


	if (m_bClick)
	{
		if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_T) & 0x80)
		{
			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_BattleTime(false);

			m_pWave_Manager->Clear_Wave();
			m_bCheckClick = true;
			m_bClick = false;
			
			if (!m_bStage1)
				m_bStage1 = true;
		}
		else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_Y) & 0x80)
		{
			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_BattleTime(false);

			m_pWave_Manager->Clear_Wave();
			m_bStage1 = false;
			m_fRegenTime0 = 0.f;
			m_fRegenTime1 = 0.f;
			m_fRegenTime2 = 0.f;
			m_fRegenTime3 = 0.f;
			m_fRegenTime4 = 0.f;
			m_fRegenTime5 = 0.f;
			m_fMonCntPA1 = 0.f;
			m_fMonCntPB1 = 0.f;
			m_fMonCntPC1 = 0.f;
			m_fMonCntPA2 = 0.f;
			m_fMonCntPB2 = 0.f;
			m_fMonCntPC2 = 0.f;

			m_bCheckClick = true;
			m_bClick = false;

			if (!m_bStage2)
				m_bStage2 = true;
		}
		else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_U) & 0x80)
		{
			m_pWave_Manager->Clear_Wave();
			m_bStage2 = false;
			m_fRegenTime0 = 0.f;
			m_fRegenTime1 = 0.f;
			m_fRegenTime2 = 0.f;
			m_fRegenTime3 = 0.f;
			m_fRegenTime4 = 0.f;
			m_fRegenTime5 = 0.f;
			m_fMonCntPA1 = 0.f;
			m_fMonCntPB1 = 0.f;
			m_fMonCntPC1 = 0.f;
			m_fMonCntPA2 = 0.f;
			m_fMonCntPB2 = 0.f;
			m_fMonCntPC2 = 0.f;

			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_BattleTime(false);

			m_bCheckClick = true;
			m_bClick = false;

			if (!m_bStage3)
				m_bStage3 = true;
		}
		else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_H) & 0x80)
		{
			m_pWave_Manager->Clear_Wave();
			m_bStage3 = false;
			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_BattleTime(false);
			m_bCheckClick = true;
			m_bClick = false;

			m_bStartMake = true;
		}
		else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_J) & 0x80)
		{
			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_BattleTime(true);

		}
		else if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_K) & 0x80)
		{
			auto& iter = m_pObject_Manager->Find_Layer(SCENE_STAGE, L"Layer_Santa")->GetObjList()->front();

			dynamic_cast<CSanta*>(iter)->Set_SuperMax();
		}
	}
	else
	{
		if (m_bCheckClick)
		{
			m_fClickTime += fTimeDelta * 1.f;
			if (m_fClickTime > 1.f)
			{
				m_fClickTime = 0.f;
				m_bClick = true;
			}
		}
	}

	if (m_bChange)
		Change_Scene();
	else if (m_bStage1)
		Ready_Stage1();
	else if(m_bStage2)
		Ready_Stage2();
	else if (m_bStage3)
		Ready_Stage3();

	if (true == m_bStage1 || true == m_bStage2 || true == m_bStage3)
		m_pColl_Manager->Check_Monster(m_pWave_Manager->Get_WaveList());

	if(m_bStartMake)
		End_MakeSnowman();
	else if(m_bBossStart)
		End_MakeBoss();

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Stage::Render_Scene(void)
{
	Engine::CScene::Render_Scene();
}

HRESULT CScene_Stage::Ready_LightInfo()
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

HRESULT CScene_Stage::Ready_Load_Object()
{
	HANDLE hFile = CreateFile(L"../Bin/Data/MapObject00.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);

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

HRESULT CScene_Stage::Ready_Layer_Object(const _tchar* pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	_uint iSize = m_vecObject.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		Engine::CGameObject*	pGameObject = nullptr;
		if (!lstrcmp(m_vecObject[i]->FileName, L"NutCracker.X") || !lstrcmp(m_vecObject[i]->FileName, L"Barrel.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"Dummy.X") || !lstrcmp(m_vecObject[i]->FileName, L"Lamp.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"Church.X") || !lstrcmp(m_vecObject[i]->FileName, L"BeerKeg.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"TreeCart.X") || !lstrcmp(m_vecObject[i]->FileName, L"Dummy.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"GardenBench.X") || !lstrcmp(m_vecObject[i]->FileName, L"OldTrimLong.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"Present.X") || !lstrcmp(m_vecObject[i]->FileName, L"PresentBag.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"SnowMan.X") || !lstrcmp(m_vecObject[i]->FileName, L"SnowMans.X")
			|| !lstrcmp(m_vecObject[i]->FileName, L"TreeLog.X") || !lstrcmp(m_vecObject[i]->FileName, L"TreeLogs.X"))
		{
			// For.Object
			pGameObject = CBaseObj::Create(m_pGraphicDev, m_vecObject[i]->FullPath,
				m_vecObject[i]->FileName, m_vecObject[i]->ComName, m_vecObject[i]->vScale, m_vecObject[i]->vPostion, m_vecObject[i]->vRotation, SCENE_STAGE);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
				return E_FAIL;
		}
		else if(!lstrcmp(m_vecObject[i]->FileName, L"ChrismasTree.X"))
		{
			// For.Tree
			pGameObject = CTree::Create(m_pGraphicDev, m_vecObject[i]->FullPath,
				m_vecObject[i]->FileName, m_vecObject[i]->ComName, m_vecObject[i]->vScale, m_vecObject[i]->vPostion, m_vecObject[i]->vRotation);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Tree", pGameObject)))
				return E_FAIL;
		}
		else
		{
			// For.NoColObj
			pGameObject = CNoCollObj::Create(m_pGraphicDev, m_vecObject[i]->FullPath,
				m_vecObject[i]->FileName, m_vecObject[i]->ComName, m_vecObject[i]->vScale, m_vecObject[i]->vPostion, m_vecObject[i]->vRotation, SCENE_STAGE);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_NoColObj", pGameObject)))
				return E_FAIL;
		}
	}

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_LeftDoor(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CDoor::Create(m_pGraphicDev, true, &_vec3(63.5f, 0.f, 64.08f), &_vec3(0.f, 270.f, 0.f));
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_RightDoor(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CDoor::Create(m_pGraphicDev, false, &_vec3(63.5f, 0.f, 62.42f), &_vec3(0.f, 270.f, 0.f));
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Boss(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CBoss::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Snowman(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = SnowmanBoss::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Santa(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.CGoblin
	pGameObject = CSanta::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Snow(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.CSnow
	pGameObject = CSnow::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Camera(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	pGameObject = CCamera_Static::Create(m_pGraphicDev, pEye, pAt, pUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pCamera_Manager->Add_Camera(L"Static_Camera", pGameObject);

	m_pCamera_Manager->Change_Camera(L"Static_Camera");

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Terrain
	pGameObject = CTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.Sky
	pGameObject = CSky::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;


	//// For.Sky
	//pGameObject = CDoorIn::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Player
	pGameObject  = CPlayer::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_TEST(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.TestNavi
	pGameObject = CTestNavi::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	_uint ia = 0;

	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 5; ++j)
		{
			_float fX = j * 53.f;
			_float fY = i * 53.f;

			_vec3 vPos = _vec3(146.0f + fX, _float(BACKSIZEY - 109.f)+ fY, 0.f);
			// For.SkillIcon
			pGameObject = CSkillIcon::Create(m_pGraphicDev, &vPos, ia);
			if (nullptr == pGameObject)
				return E_FAIL;

			if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
				return E_FAIL;

			++ia;
		}
	}


	// For.HpBar
	pGameObject = CHpBar::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.HpBar
	pGameObject = CMpBar::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.CPanel
	pGameObject = CPanel::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;


	// For.CPanel
	pGameObject = CAim::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STATIC, pLayerTag, pGameObject)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CScene_Stage::Change_Scene()
{
	auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();

	dynamic_cast<CPlayer*>(iter)->Set_UpdateBool(true);

	Engine::CScene*			pNewScene = nullptr;

	pNewScene = CBossLogo::Create(m_pGraphicDev);
	if (nullptr == pNewScene)
		return E_FAIL;

	if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Stage1()
{
	if (nullptr == m_pObject_Manager || nullptr == m_pWave_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	//if (m_bWaveStart)
	//{
	//	m_bWaveStart = false;
	//	pGameObject = CPhase::Create(m_pGraphicDev, 1, SCENE_STAGE);
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Phase", pGameObject)))
	//		return E_FAIL;
	//}

	m_fRegenTime0 += m_fTimeDeta;
	if (m_fRegenTime0 > 3.5f && m_fMonCntPA1 < 3)
	{
		m_fRegenTime0 = 0.f;
		++m_fMonCntPA1;
		// For.CGoblin
		pGameObject = CGoblin::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Goblin", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime1 += m_fTimeDeta;
	if (m_fRegenTime1 > 3.5f && m_fMonCntPA2 < 2)
	{
		m_fRegenTime1 = 0.f;
		++m_fMonCntPA2;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}


	m_fRegenTime2 += m_fTimeDeta;
	if (m_fRegenTime2 > 4.5f && m_fMonCntPB1 < 2)
	{
		m_fRegenTime2 = 0.f;
		++m_fMonCntPB1;
		// For.CGoblin
		pGameObject = CGoblin::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Goblin", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime3 += m_fTimeDeta;
	if (m_fRegenTime3 > 2.5f && m_fMonCntPB2 < 2)
	{
		m_fRegenTime1 = 0.f;
		++m_fMonCntPB2;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime4 += m_fTimeDeta;
	if (m_fRegenTime4 > 3.5f && m_fMonCntPC2 < 2)
	{
		m_fRegenTime4 = 0.f;
		++m_fMonCntPC2;
		// For.CGoblin
		pGameObject = CGoblin::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Goblin", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime5 += m_fTimeDeta;
	if (m_fRegenTime5 > 2.5f && m_fMonCntPC1 < 2)
	{
		m_fRegenTime5 = 0.f;
		++m_fMonCntPC1;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	if (m_fMonCntPA1 == 2 && m_fMonCntPB1 == 2 && m_fMonCntPC1 == 2
		&& m_fMonCntPA2 == 2 && m_fMonCntPB2 == 2 && m_fMonCntPC2 == 2)
	{
		m_bStage1 = false;
		m_bWaveStart = true;
	}
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Stage2()
{
	if (nullptr == m_pObject_Manager || nullptr == m_pWave_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	m_fRegenTime0 += m_fTimeDeta;
	if (m_fRegenTime0 > 3.5f && m_fMonCntPA1 < 2)
	{
		m_fRegenTime0 = 0.f;
		++m_fMonCntPA1;
		// For.CGoblin
		pGameObject = CKobold::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime1 += m_fTimeDeta;
	if (m_fRegenTime1 > 2.5f && m_fMonCntPA2 < 1)
	{
		m_fRegenTime1 = 0.f;
		++m_fMonCntPA2;
		// For.CArcher
		pGameObject = CGoblin::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime2 += m_fTimeDeta;
	if (m_fRegenTime2 > 2.5f && m_fMonCntPB1 < 2)
	{
		m_fRegenTime2 = 0.f;
		++m_fMonCntPB1;
		// For.CGoblin
		pGameObject = CKobold::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime3 += m_fTimeDeta;
	if (m_fRegenTime3 > 2.5f && m_fMonCntPB2 < 3)
	{
		m_fRegenTime3 = 0.f;
		++m_fMonCntPB2;
		// For.CArcher
		pGameObject = CGoblin::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime4 += m_fTimeDeta;
	if (m_fRegenTime4 > 3.5f && m_fMonCntPC1 < 2)
	{
		m_fRegenTime4 = 0.f;
		++m_fMonCntPC1;
		// For.CGoblin
		pGameObject = CKobold::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime5 += m_fTimeDeta;
	if (m_fRegenTime5 > 2.5f && m_fMonCntPC2 < 2)
	{
		m_fRegenTime5 = 0.f;
		++m_fMonCntPC2;
		// For.CArcher
		pGameObject = CGoblin::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	if (m_fMonCntPA1 == 2 && m_fMonCntPB1 == 2 && m_fMonCntPC1 == 2
		&& m_fMonCntPA2 == 2 && m_fMonCntPB2 == 2 && m_fMonCntPC2 == 2)
		m_bStage2 = false;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Stage3()
{
	if (nullptr == m_pObject_Manager || nullptr == m_pWave_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	m_fRegenTime0 += m_fTimeDeta;
	if (m_fRegenTime0 > 3.5f && m_fMonCntPA1 < 2)
	{
		m_fRegenTime0 = 0.f;
		++m_fMonCntPA1;
		// For.CGoblin
		pGameObject = COrcBruiser::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime1 += m_fTimeDeta;
	if (m_fRegenTime1 > 2.5f && m_fMonCntPA2 < 3)
	{
		m_fRegenTime1 = 0.f;
		++m_fMonCntPA2;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 0);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_A, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime2 += m_fTimeDeta;
	if (m_fRegenTime2 > 3.5f && m_fMonCntPB1 < 2)
	{
		m_fRegenTime2 = 0.f;
		++m_fMonCntPB1;
		// For.CGoblin
		pGameObject = COrcBruiser::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime3 += m_fTimeDeta;
	if (m_fRegenTime3 > 1.5f && m_fMonCntPB2 < 2)
	{
		m_fRegenTime3 = 0.f;
		++m_fMonCntPB2;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 1);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_B, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime4 += m_fTimeDeta;
	if (m_fRegenTime4 > 3.5f && m_fMonCntPC1 < 2)
	{
		m_fRegenTime4 = 0.f;
		++m_fMonCntPC1;
		// For.CGoblin
		pGameObject = COrcBruiser::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Kobold", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	m_fRegenTime5 += m_fTimeDeta;
	if (m_fRegenTime5 > 2.5f && m_fMonCntPC2 < 2)
	{
		m_fRegenTime5 = 0.f;
		++m_fMonCntPC2;
		// For.CArcher
		pGameObject = CDarkElfArcher::Create(m_pGraphicDev, 2);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Archer", pGameObject)))
			return E_FAIL;
		if (FAILED(m_pWave_Manager->Add_WaveMonster(CWave_Manager::POINT_C, pGameObject)))
			return E_FAIL;
	}

	if (m_fMonCntPA1 == 2 && m_fMonCntPB1 == 2 && m_fMonCntPC1 == 2
		&& m_fMonCntPA2 == 2 && m_fMonCntPB2 == 2 && m_fMonCntPC2 == 2)
		m_bStage3 = false;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Snowman()
{
	if (nullptr == m_pObject_Manager || nullptr == m_pWave_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	pGameObject = SnowmanBoss::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Snowman", pGameObject)))
		return E_FAIL;

	m_bMakeEnd = false;

	return NOERROR;
}

HRESULT CScene_Stage::Move_SantaTime()
{
	return NOERROR;
}

HRESULT CScene_Stage::Add_BGM_ForStage()
{
	CSound_Manager::GetInstance()->Stop_Sound(CSound_Manager::Channel_BGM);
	CSound_Manager::GetInstance()->Play_BGM(L"Mountain.mp3", CSound_Manager::Channel_BGM, 0.8f);

	return NOERROR;
}

void CScene_Stage::End_MakeSnowman()
{
	if (!m_bChangeCam)
	{
		m_bChangeCam = true;
		CCamera_Manager::GetInstance()->Change_Camera(L"Dynamic_Camera");
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();

		CTransform* pObject = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");

		_vec3 vPlayerDir = pObject->Get_Look();
		_vec3 vPlayerPos = pObject->Get_Position();
		_vec3 vPos = vPlayerPos - vPlayerDir * 2.0f;
		vPos.y += 1.4f;

		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CameraEye(_vec3(52.f, 7.f, 57.8f));

		vPlayerPos.z -= 4.f;

		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_At(&_vec3(48.f, 7.f, 63.8f));
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_EventStart(true);

		CGameObject* pBoss = m_pObject_Manager->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
		dynamic_cast<CBoss*>(pBoss)->Set_MakeSnowMan(true);
		dynamic_cast<CBoss*>(pBoss)->Set_StartSnowMan(true);
	}

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();

	CGameObject* pBoss = m_pObject_Manager->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
	CTransform* pTrans = (CTransform*)dynamic_cast<CBoss*>(pBoss)->Get_Component(L"Com_Transform");
	_vec3 vBossPos =pTrans->Get_Position();

	dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_At(&vBossPos);
}

void CScene_Stage::End_MakeBoss()
{
	if (!m_bChangeCam)
	{
		m_bChangeCam = true;
		CCamera_Manager::GetInstance()->Change_Camera(L"Dynamic_Camera");
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();

		CTransform* pObject = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");

		_vec3 vPlayerDir = pObject->Get_Right();
		_vec3 vPlayerPos = _vec3(45.f, 0.f, 60.f);
		_vec3 vPos = vPlayerPos - vPlayerDir * 5.0f;
		vPos.y += 1.4f;

		pCamera->Set_CameraEye(vPos);

		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_At(&_vec3(48.f, 7.f, 63.8f));
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_EventStart(true);

		CGameObject* pBoss = m_pObject_Manager->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
		dynamic_cast<CBoss*>(pBoss)->Set_MakeSnowMan(true);
		dynamic_cast<CBoss*>(pBoss)->Set_StartBoss(true);
	}

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_CurCamera();

	CGameObject* pBoss = m_pObject_Manager->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
	CTransform* pTrans = (CTransform*)dynamic_cast<CBoss*>(pBoss)->Get_Component(L"Com_Transform");
	_vec3 vBossPos = pTrans->Get_Position();

	dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_At(&vBossPos);
}


CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Stage *	pInstance = new CScene_Stage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Stage Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CScene_Stage::Free(void)
{
	CSound_Manager::GetInstance()->Stop_Sound(CSound_Manager::Channel_BGM);

	auto& iter = m_vecObject.begin();
	auto& iter_end = m_vecObject.end();
	for (; iter != iter_end; ++iter)
	{
		Safe_Delete((*iter));
	}
	m_vecObject.clear();

	Engine::Safe_Release(m_pWave_Manager);

	if (FAILED(CScene::Release_Object(SCENE_STAGE)))
		MSG_BOX("Scene_Logo Release Failed");

	if (FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_STAGE)))
		MSG_BOX("Scene_Logo Release Failed");

	return Engine::CScene::Free();
}

