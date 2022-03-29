#include "stdafx.h"
#include "..\Headers\Loading.h"

#include "Component_Manager.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

unsigned APIENTRY Thread_Main(void* pArgList)
{
	CLoading* pLoading = (CLoading*)pArgList;

	EnterCriticalSection(pLoading->Get_Key());

	switch (pLoading->Get_SceneID())
	{
	case SCENE_STAGE:
		pLoading->Loading_ForStageScene();
		break;
	case SCENE_BOSS:
		pLoading->Loading_ForBossScene();
		break;
	}
	LeaveCriticalSection(pLoading->Get_Key());
	pLoading->Set_Finish(true);
	return 0;
}
HRESULT CLoading::Ready_Loading(SCENETYPE eSceneID)
{
	m_eSceneID = eSceneID;

	InitializeCriticalSection(&m_Key);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return NOERROR;
}

HRESULT CLoading::Loading_ForStageScene(void)
{
	// For.Stage_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	/* ********************************************Buffer******************************************** */
	/* Material */
	lstrcpy(m_szLoading, L"Material Loading.");
	// For.Material
	pComponent = Engine::CMaterial::Create(m_pGraphicDev, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 10);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Material", pComponent)))
		return E_FAIL;

	/* Terrain */
	lstrcpy(m_szLoading, L"Terrain Buffer Loading......");
	// For.Buffer_Terrain
	pComponent = Engine::CTerrain_Buffer::Create(m_pGraphicDev, 129, 129, 1/*,  L"../Bin/Resources/Textures/Terrain/Height.bmp"*/);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Terrain", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_RightCol", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Collider", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_LeftCol", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_SnowCol", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/MapNavi00.dat");
	//pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/TestNavi4.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Navigation", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/SantaNavi.dat");
	//pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/TestNavi4.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Navigation_Santa", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/Stage_PA.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Navigation_PointA", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/Stage_PB.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Navigation_PointB", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/Stage_PC.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Navigation_PointC", pComponent)))
		return E_FAIL;

	// for.Component_Frustum
	lstrcpy(m_szLoading, L"Frustum Loading.");
	pComponent = CFrustum::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Frustum", pComponent)))
		return E_FAIL;

	// for.Component_SnowPaticle
	lstrcpy(m_szLoading, L"SnowPaticle Loading.");
	pComponent = CSnowPaticle::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Particle/Snow.dds", 1600);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Snow_Paticle", pComponent)))
		return E_FAIL;

	//L"../Bin/Resources/Textures/Particle/Particle_8.tga"
	// for.Component_BossPaticle
	lstrcpy(m_szLoading, L"SnowPaticle Loading.");
	pComponent = CBossPaticle::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Particle/Particle_8.tga", 200);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Boss_Paticle", pComponent)))
		return E_FAIL;

	/*********************************************Texture*********************************************/

	/* Terrain Texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-D.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Terrain", pComponent)))
		return E_FAIL;

	/* Terrain Texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-N.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Terrain_Normal", pComponent)))
		return E_FAIL;

	/* TowerRadius Texture */
	lstrcpy(m_szLoading, L"TowerRange Texture Loading..");
	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/TowerRadius/towerBuildRange.tga");
	//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Aura0.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_TowerRadius", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-D.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowBase.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_SnowBase", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"DoorIn Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/DoorIn/DoorIn.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_DoorIn", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMix02.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Rocks", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMix02_NRM.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Rocks_Normal", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMud.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Multi0", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/BrickedMarble1_d.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_BrickedMarble1_d", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Floor1_d.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Floor1_d", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Explosion/explosionFlipbook_%d.png", 16);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_Explosion", pComponent)))
		return E_FAIL;

	/* ********************************************SkyBox******************************************** */
	/* SkyBox Texture */
	lstrcpy(m_szLoading, L"Sky Texture Loading...");
	// For.Texture_Sky
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_CUBE, L"../Bin/Resources/Textures/SkyBox/Skybox.dds");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Sky", pComponent)))
		return E_FAIL;
	/* SkyBox Buffer == CubeBuffer */
	lstrcpy(m_szLoading, L"Cube Texture Loading......");
	// For.Buffer_CubeTexture
	pComponent = Engine::CCube_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Sky", pComponent)))
		return E_FAIL;


	///* ********************************************Static Mesh******************************************** */
	/* CrossBow */
	lstrcpy(m_szLoading, L"CrossBow Mesh Loading......");
	// For.Component_Mesh_Bow
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/CrossBow/", L"CrossBow.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Mesh_CrossBow", pComponent)))
		return E_FAIL;

	/* Bolt */
	lstrcpy(m_szLoading, L"Bolt Mesh Loading......");
	// For.Component_Mesh_Bow
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Bolt/", L"Bolt.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Mesh_Bolt", pComponent)))
		return E_FAIL;

	/* Arrow */
	// For.Component_Mesh_Arrow
	lstrcpy(m_szLoading, L"Arrow Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Arrow/", L"Arrow.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Arrow", pComponent)))
		return E_FAIL;

	/* ChrismasTree */
	lstrcpy(m_szLoading, L"ChrismasTree Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/ChrismasTree/", L"ChrismasTree.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_ChrismasTree", pComponent)))
		return E_FAIL;
	/* Tomstone */
	lstrcpy(m_szLoading, L"Tomstone Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/TombStone/", L"TombStone.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_TestObj", pComponent)))
		return E_FAIL;
	/* Church */
	lstrcpy(m_szLoading, L"Church Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Church/", L"Church.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Church", pComponent)))
		return E_FAIL;
	/* SnowMan */
	lstrcpy(m_szLoading, L"SnowMan Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/SnowMan/", L"SnowMan.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_SnowMan", pComponent)))
		return E_FAIL;
	/* SnowMans */
	lstrcpy(m_szLoading, L"SnowMans Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/SnowMans/", L"SnowMans.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_SnowMans", pComponent)))
		return E_FAIL;
	/* SnowBall */
	lstrcpy(m_szLoading, L"SnowBall Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/SnowBall/", L"SnowBall.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_SnowBall", pComponent)))
		return E_FAIL;
	/* GardenBench */
	lstrcpy(m_szLoading, L"GardenBench Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/GardenBench/", L"GardenBench.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_GardenBench", pComponent)))
		return E_FAIL;
	/* TreeLog */
	lstrcpy(m_szLoading, L"TreeLog Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/TreeLog/", L"TreeLog.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_TreeLog", pComponent)))
		return E_FAIL;
	/* TreeLogs */
	lstrcpy(m_szLoading, L"TreeLogs Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/TreeLogs/", L"TreeLogs.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_TreeLogs", pComponent)))
		return E_FAIL;
	/* TreeCart */
	lstrcpy(m_szLoading, L"TreeCart Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Cart/", L"TreeCart.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_TreeCart", pComponent)))
		return E_FAIL;
	/* Barrel */
	lstrcpy(m_szLoading, L"Barrel Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Barrel/", L"Barrel.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Barrel", pComponent)))
		return E_FAIL;
	/* NutCrackers */
	lstrcpy(m_szLoading, L"NutCrackers Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/NutCrackers/", L"NutCracker.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_NutCracker", pComponent)))
		return E_FAIL;
	/* OldTrimLong */
	lstrcpy(m_szLoading, L"OldTrimLong Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/OldTrimLong/", L"OldTrimLong.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_OldTrimLong", pComponent)))
		return E_FAIL;
	/* PresentBag */
	lstrcpy(m_szLoading, L"PresentBag Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentBag/", L"PresentBag.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_PresentBag", pComponent)))
		return E_FAIL;
	/* Present */
	lstrcpy(m_szLoading, L"Present Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Present/", L"Present.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Present", pComponent)))
		return E_FAIL;
	/* Dummy */
	lstrcpy(m_szLoading, L"Dummy Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Dummy/", L"Dummy.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Dummy", pComponent)))
		return E_FAIL;
	/* BeerKeg */
	lstrcpy(m_szLoading, L"BeerKeg Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/BeerKeg/", L"BeerKeg.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_BeerKeg", pComponent)))
		return E_FAIL;
	/* Lamp */
	lstrcpy(m_szLoading, L"Lamp Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Lamp/", L"Lamp.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Lamp", pComponent)))
		return E_FAIL;
	/* LeftDoor */
	lstrcpy(m_szLoading, L"LeftDoor Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/LeftDoor/", L"LeftDoor.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_LeftDoor", pComponent)))
		return E_FAIL;
	/* RightDoor */
	lstrcpy(m_szLoading, L"RightDoor Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/RightDoor/", L"RightDoor.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_RightDoor", pComponent)))
		return E_FAIL;



	/* ********************************************Dynamic Mesh*********************************************************/
	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_Player
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/SuperHuntress/", L"SuperHuntress.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Mesh_Player", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_LightingTower
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/LightningTower/", L"LightningTower.X", _vec3(0.f, 0.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_LightningTower", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_Harbinger
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Harbinger/", L"Harbinger.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Boss", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/SnowmanBoss/", L"SnowmanBoss.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_SnowMansBoss", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Goblin/", L"Goblin.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Goblin", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Kobold/", L"Kobold.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Kobold", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/DarkElfArcher/", L"DarkElfArcher.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_DarkElfArcher", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/OrcBruiser/", L"OrcBruiser.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_OrcBruiser", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/StrikerTower/", L"StrikerTower.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_StrikerTower", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/MissleTower/", L"MissleTower.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_MissleTower", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/MageBlockade/", L"MageBlockade.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Shield", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Santa/", L"Santa.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_Santa", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/DragonKiller/", L"DragonKiller.X", _vec3(0.f, 0.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_DragonKiller", pComponent)))
		return E_FAIL;



	/********************************************* Shader********************************************  */
	// For.Component_Shader_Terrain
	//lstrcpy(m_szLoading, L"Terrain_Shader Loading......");
	//pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Terrain.fx");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Terrain", pComponent)))
	//	return E_FAIL;

	lstrcpy(m_szLoading, L"Terrain_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Terrain_Not.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Terrain", pComponent)))
		return E_FAIL;


	// For.Component_Shader_Cube
	lstrcpy(m_szLoading, L"Cube Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Cube.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Cube", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Mesh
	lstrcpy(m_szLoading, L"Mesh_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Mesh.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Mesh", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Mesh
	lstrcpy(m_szLoading, L"Particle_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Particle.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Particle", pComponent)))
		return E_FAIL;

	// For.Component_Shader_eFFECT
	lstrcpy(m_szLoading, L"Mesh_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Effect.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Shader_Effect", pComponent)))
		return E_FAIL;

	// For.Component_Shader_UI
	lstrcpy(m_szLoading, L"Mesh_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_UI.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Shader_UI", pComponent)))
		return E_FAIL;


	// Effect
	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/FlameB/FlameB%d.tga", 30);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_FlameEffect", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/MonsterUD002_Tex.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_FlameBall", pComponent)))
		return E_FAIL;

	/* Bullert */
	lstrcpy(m_szLoading, L"Bullet Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/Bullet/", L"Bullet.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_Bullet", pComponent)))
		return E_FAIL;

	/* BullertTrail */
	lstrcpy(m_szLoading, L"BulletTrail Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/Effect/Projectile1/", L"Projectile1.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_BulletTrail", pComponent)))
		return E_FAIL;

	/* BullertTrail */
	lstrcpy(m_szLoading, L"BulletTrail Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/Effect/Head_Projectile/", L"Head_Projectile.X");
	if (nullptr == pComponent)	
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Buffer_WindCutter", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Fire/Fireball_%d.png", 3);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Fire", pComponent)))
		return E_FAIL;

	/* EnergyBall */
	lstrcpy(m_szLoading, L"EnergyBall Mesh Loading......");
	// For.Component_Mesh_Bow  // Bin\Resources\Textures\Effect\LightingBall
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/LightingBall/", L"LightingBall.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_EnergyBall", pComponent)))
		return E_FAIL;

	/* FireBall */
	lstrcpy(m_szLoading, L"EnergyBall Mesh Loading......");
	// For.Component_Mesh_Bow  // Bin\Resources\Textures\Effect\LightingBall
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/FireBall/", L"FireBall.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Mesh_FireBall", pComponent)))
		return E_FAIL;

	/////////////////UI///////////////////////////
	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Panel/Panel_SkillIcon2.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_Panel", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/SKillIcon/%d.tga", 10);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_Skill", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Hp/RedGage.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_RedGage", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Text/Phase/%d.tga", 2);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"Component_Texture_Phase", pComponent)))
		return E_FAIL;


	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Hp/BuleGage.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_BuleGage", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Aim/Aim.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_Aim", pComponent)))
		return E_FAIL;


	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/End/SpecialThanks2.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Texture_End", pComponent)))
		return E_FAIL;


	lstrcpy(m_szLoading, L"End!!!");

	return NOERROR;
}

HRESULT CLoading::Loading_ForBossScene(void)
{
	// For.Stage_Scene Component
	Engine::CComponent*			pComponent = nullptr;


	/* ********************************************Buffer******************************************** */
	/* Material */
	lstrcpy(m_szLoading, L"Material Loading.");
	// For.Material
	pComponent = Engine::CMaterial::Create(m_pGraphicDev, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 10);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Material", pComponent)))
		return E_FAIL;

	/* EnergyBall */
	lstrcpy(m_szLoading, L"EnergyBall Mesh Loading......");
	// For.Component_Mesh_Bow  // Bin\Resources\Textures\Effect\LightingBall
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/LightingBall/", L"LightingBall.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_EnergyBall", pComponent)))
		return E_FAIL;

	/* Terrain */
	lstrcpy(m_szLoading, L"Terrain Buffer Loading......");
	// For.Buffer_Terrain
	pComponent = Engine::CTerrain_Buffer::Create(m_pGraphicDev, 129, 129, 1/*,  L"../Bin/Resources/Textures/Terrain/Height.bmp"*/);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Terrain", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Collider", pComponent)))
		return E_FAIL;

	// For.Navigation
	lstrcpy(m_szLoading, L"Navigation Loading");
	//pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/MapNavi00.dat");
	pComponent = CNavigation::Create(m_pGraphicDev, L"../Bin/Data/Test8.dat");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Navigation", pComponent)))
		return E_FAIL;

	// for.Component_Frustum
	lstrcpy(m_szLoading, L"Frustum Loading.");
	pComponent = CFrustum::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Frustum", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_RightCol", pComponent)))
		return E_FAIL;

	/* Collider */
	lstrcpy(m_szLoading, L"ColliderCom Loading......");
	// For.Buffer_Collider
	pComponent = Engine::CCollider::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_LeftCol", pComponent)))
		return E_FAIL;

	/*********************************************Texture*********************************************/

	/* Terrain Texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-D.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Terrain", pComponent)))
		return E_FAIL;

	/* TowerRadius Texture */
	lstrcpy(m_szLoading, L"TowerRange Texture Loading..");
	// For.Texture_Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/TowerRadius/towerBuildRange.tga");
	//pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Aura0.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_TowerRadius", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Snow-D.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowBase.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_SnowBase", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMix02.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Rocks", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/SnowMud.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Multi0", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/BrickedMarble1_d.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_BrickedMarble1_d", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Terrain/Floor1_d.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Floor1_d", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Text/Phase/3.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Phase", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Text/Boss/1.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_BossPhase", pComponent)))
		return E_FAIL;

	/* ********************************************SkyBox******************************************** */
	/* SkyBox Texture */
	lstrcpy(m_szLoading, L"Sky Texture Loading...");
	// For.Texture_Sky
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_CUBE, L"../Bin/Resources/Textures/SkyBox/Skybox.dds");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Sky", pComponent)))
		return E_FAIL;
	/* SkyBox Buffer == CubeBuffer */
	lstrcpy(m_szLoading, L"Cube Texture Loading......");
	// For.Buffer_CubeTexture
	pComponent = Engine::CCube_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Sky", pComponent)))
		return E_FAIL;

	/* ********************************************Dynamic Mesh*********************************************************/

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_Player
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/LightningTower/", L"LightningTower.X", _vec3(0.f, 0.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_LightningTower", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/DragonKiller/", L"DragonKiller.X", _vec3(0.f, 0.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_DragonKiller", pComponent)))
		return E_FAIL;

		lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/MissleTower/", L"MissleTower.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_MissleTower", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_SnowMans
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/MageBlockade/", L"MageBlockade.X", _vec3(0.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_Shield", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_Harbinger
	pComponent = Engine::CDynamic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Harbinger/", L"Harbinger.X", _vec3(270.f, 180.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_Boss", pComponent)))
		return E_FAIL;


	lstrcpy(m_szLoading, L"Dynamcic Mesh Loading......");
	// For.Component_Mesh_Harbinger
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Crystal/", L"Crystal.X"/*, _vec3(270.f, 180.f, 0.f)*/);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_Crystal", pComponent)))
		return E_FAIL;

	/********************************************* Shader********************************************  */
	lstrcpy(m_szLoading, L"Terrain_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Terrain_Not.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Shader_Terrain", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Cube
	lstrcpy(m_szLoading, L"Cube Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Cube.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Shader_Cube", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Mesh
	lstrcpy(m_szLoading, L"Mesh_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Mesh.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Shader_Mesh", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Mesh
	lstrcpy(m_szLoading, L"Particle_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Particle.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Shader_Particle", pComponent)))
		return E_FAIL;

	// For.Component_Shader_eFFECT
	lstrcpy(m_szLoading, L"Mesh_Shader Loading......");
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Effect.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Shader_Effect", pComponent)))
		return E_FAIL;

	/* ************************* Test**************************************   */

	/* Wall01 */
	lstrcpy(m_szLoading, L"Test Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Wall01/", L"Wall01.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Wall01", pComponent)))
		return E_FAIL;

	/* Railing */
	lstrcpy(m_szLoading, L"Test Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Railing/", L"Railing.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Railing", pComponent)))
		return E_FAIL;

	/* WoodRailingGroup_01 */
	lstrcpy(m_szLoading, L"Test Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/WoodRailingGroup_01/", L"WoodRailingGroup_01.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_WoodRailingGroup_01", pComponent)))
		return E_FAIL;

	/* StairGroup_02 */
	lstrcpy(m_szLoading, L"Test Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/StairGroup_02/", L"StairGroup_02.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_StairGroup_02", pComponent)))
		return E_FAIL;

	/* ArcanePad_HI */
	lstrcpy(m_szLoading, L"Test Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/ArcanePad_HI/", L"ArcanePad_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_ArcanePad_HI", pComponent)))
		return E_FAIL;

	/* BookcaseGroup_01 */
	lstrcpy(m_szLoading, L"BookcaseGroup_01 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/BookcaseGroup_01/", L"BookcaseGroup_01.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_BookcaseGroup_01", pComponent)))
		return E_FAIL;

	/* BookcaseGroup_02 */
	lstrcpy(m_szLoading, L"BookcaseGroup_02 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/BookcaseGroup_02/", L"BookcaseGroup_02.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_BookcaseGroup_02", pComponent)))
		return E_FAIL;

	/* BookcaseGroup_03 */
	lstrcpy(m_szLoading, L"BookcaseGroup_03 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/BookcaseGroup_03/", L"BookcaseGroup_03.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_BookcaseGroup_03", pComponent)))
		return E_FAIL;

	/* BookcaseGroup_04 */
	lstrcpy(m_szLoading, L"BookcaseGroup_04 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/BookcaseGroup_04/", L"BookcaseGroup_04.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_BookcaseGroup_04", pComponent)))
		return E_FAIL;

	/* Group_04 */
	lstrcpy(m_szLoading, L"Group_04 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_04/", L"Group_04.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_04", pComponent)))
		return E_FAIL;

	/* Group_37 */
	lstrcpy(m_szLoading, L"Group_37 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_37/", L"Group_37.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_37", pComponent)))
		return E_FAIL;

	/* Group_38 */
	lstrcpy(m_szLoading, L"Group_38 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_38/", L"Group_38.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_38", pComponent)))
		return E_FAIL;

	/* Group_49 */
	lstrcpy(m_szLoading, L"Group_49 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_49/", L"Group_49.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_49", pComponent)))
		return E_FAIL;

	/* Group_36 */
	lstrcpy(m_szLoading, L"Group_36 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_36/", L"Group_36.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_36", pComponent)))
		return E_FAIL;

	/* Group_25 */
	lstrcpy(m_szLoading, L"Group_25 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_25/", L"Group_25.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_25", pComponent)))
		return E_FAIL;

	/* Group_16 */
	lstrcpy(m_szLoading, L"Group_16 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_16/", L"Group_16.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_16", pComponent)))
		return E_FAIL;

	/* Group_8 */
	lstrcpy(m_szLoading, L"Group_8 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Group_08/", L"Group_08.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Group_08", pComponent)))
		return E_FAIL;

	/* Group_8 */
	lstrcpy(m_szLoading, L"magusQuartersRoof_SM Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/magusQuartersRoof_SM/", L"magusQuartersRoof_SM.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_magusQuartersRoof_SM", pComponent)))
		return E_FAIL;

	/* Book_CollectionShape2_HI */
	lstrcpy(m_szLoading, L"Book_CollectionShape2_HI Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Book_CollectionShape2_HI/", L"Book_CollectionShape2_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Book_CollectionShape2_HI", pComponent)))
		return E_FAIL;

	/* Book_Green_HI */
	lstrcpy(m_szLoading, L"Book_Green_HI Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Book_Green_HI/", L"Book_Green_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Book_Green_HI", pComponent)))
		return E_FAIL;

	/* Pillar */
	lstrcpy(m_szLoading, L"Pillar Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Pillar/", L"Pillar.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Pillar", pComponent)))
		return E_FAIL;

	/* LionStatue */
	lstrcpy(m_szLoading, L"LionStatue Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/LionStatue/", L"LionStatue.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_LionStatue", pComponent)))
		return E_FAIL;

	/* StoneKnight */
	lstrcpy(m_szLoading, L"StoneKnight Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/StoneKnight/", L"StoneKnight.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_StoneKnight", pComponent)))
		return E_FAIL;

	/* CoreGeo_part_00 */
	lstrcpy(m_szLoading, L"CoreGeo_part_00 Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/CoreGeo_part_00/", L"CoreGeo_part_00.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_CoreGeo_part_00", pComponent)))
		return E_FAIL;

	/* Treasure_Vase */
	lstrcpy(m_szLoading, L"Treasure_Vase Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Treasure_Vase/", L"Treasure_Vase.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Treasure_Vase", pComponent)))
		return E_FAIL;

	/* WallTorch_HI */
	lstrcpy(m_szLoading, L"WallTorch_HI Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/WallTorch_HI/", L"WallTorch_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_WallTorch_HI", pComponent)))
		return E_FAIL;

	/* WallBanner_Large_HI */
	lstrcpy(m_szLoading, L"WallBanner_Large_HI Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/WallBanner_Large_HI/", L"WallBanner_Large_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_WallBanner_Large_HI", pComponent)))
		return E_FAIL;

	/* Wall_Banner01A */
	lstrcpy(m_szLoading, L"Wall_Banner01A Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Wall_Banner01A/", L"Wall_Banner01A.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Wall_Banner01A", pComponent)))
		return E_FAIL;

	/* Bookcase_HI */
	lstrcpy(m_szLoading, L"Bookcase_HI Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Bookcase_HI/", L"Bookcase_HI.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Bookcase_HI", pComponent)))
		return E_FAIL;

	/* Candelabra */
	lstrcpy(m_szLoading, L"Candelabra Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Candelabra/", L"Candelabra.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Candelabra", pComponent)))
		return E_FAIL;

	/* CandleLamp */
	lstrcpy(m_szLoading, L"CandleLamp Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/CandleLamp/", L"CandleLamp.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_CandleLamp", pComponent)))
		return E_FAIL;

	/* ChristmasSleigh */
	lstrcpy(m_szLoading, L"ChristmasSleigh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/ChristmasSleigh/", L"ChristmasSleigh.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_ChristmasSleigh", pComponent)))
		return E_FAIL;

	/* Painting */
	lstrcpy(m_szLoading, L"Painting Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Painting/", L"Painting.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Painting", pComponent)))
		return E_FAIL;

	/* Present */
	lstrcpy(m_szLoading, L"Present Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Present/", L"Present.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Present", pComponent)))
		return E_FAIL;

	/* PresentA */
	lstrcpy(m_szLoading, L"PresentA Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentA/", L"PresentA.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentA", pComponent)))
		return E_FAIL;

	/* PresentB */
	lstrcpy(m_szLoading, L"PresentB Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentB/", L"PresentB.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentB", pComponent)))
		return E_FAIL;

	/* PresentC */
	lstrcpy(m_szLoading, L"PresentC Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentC/", L"PresentC.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentC", pComponent)))
		return E_FAIL;

	/* PresentD */
	lstrcpy(m_szLoading, L"PresentD Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentD/", L"PresentD.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentD", pComponent)))
		return E_FAIL;

	/* PresentF */
	lstrcpy(m_szLoading, L"PresentF Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentF/", L"PresentF.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentF", pComponent)))
		return E_FAIL;

	/* PresentG */
	lstrcpy(m_szLoading, L"PresentG Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentG/", L"PresentG.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentG", pComponent)))
		return E_FAIL;

	/* PresentBag */
	lstrcpy(m_szLoading, L"PresentBag Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/PresentBag/", L"PresentBag.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_PresentBag", pComponent)))
		return E_FAIL;

	/* Arrow */
	// For.Component_Mesh_Arrow
	lstrcpy(m_szLoading, L"Arrow Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Arrow/", L"Arrow.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_Arrow", pComponent)))
		return E_FAIL;

	/* NutCrackers */
	lstrcpy(m_szLoading, L"NutCrackers Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/NutCrackers/", L"NutCracker.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_NutCracker", pComponent)))
		return E_FAIL;

	/* Barrel */
	lstrcpy(m_szLoading, L"Barrel Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Barrel/", L"Barrel.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Barrel", pComponent)))
		return E_FAIL;

	// Effect
	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/FlameB/FlameB%d.tga", 30);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_FlameEffect", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/MonsterUD002_Tex.tga");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_FlameBall", pComponent)))
		return E_FAIL;
	/* FireBall */

	lstrcpy(m_szLoading, L"EnergyBall Mesh Loading......");
	// For.Component_Mesh_Bow  // Bin\Resources\Textures\Effect\LightingBall
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/FireBall/", L"FireBall.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Mesh_FireBall", pComponent)))
		return E_FAIL;

	/* Bullert */
	lstrcpy(m_szLoading, L"Bullet Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/Bullet/", L"Bullet.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_Bullet", pComponent)))
		return E_FAIL;

	/* texture */
	lstrcpy(m_szLoading, L"Terrain Texture Loading..");
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/Effect/Fire/Fireball_%d.png", 3);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Texture_Fire", pComponent)))
		return E_FAIL;

	/* BullertTrail */
	lstrcpy(m_szLoading, L"BulletTrail Mesh Loading......");
	pComponent = Engine::CStatic_Mesh::Create(m_pGraphicDev, L"../Bin/Resources/Textures/Effect/Effect/Head_Projectile/", L"Head_Projectile.X");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_BOSS, L"Component_Buffer_WindCutter", pComponent)))
		return E_FAIL;

	lstrcpy(m_szLoading, L"End!!!");

	return NOERROR;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eSceneID)
{
	CLoading *	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eSceneID)))
	{
		MSG_BOX("CScene_Stage Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pGraphicDev);

	DeleteCriticalSection(&m_Key);

	CloseHandle(m_hThread);

	return 0;
}
