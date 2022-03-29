#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"

#include "Back_Logo.h"
#include "MainBack_Logo.h"
#include "Component_manager.h"
#include "Camera_Dynamic.h"
#include "Input_Device.h"
#include "Scene_Stage.h"
#include "Management.h"

#include "Graphic_Device.h"
#include "Loading.h"

#include "Sound_Manager.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

HRESULT CScene_Logo::Ready_Scene(void)
{
	if (FAILED(Ready_D3dFont()))
		return E_FAIL;

	if (FAILED(Add_Logo_BGM()))
		return E_FAIL;

	if (FAILED(Ready_LogoComponent()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(&_vec3(0.f, 0.f, -5.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphicDev, SCENE_STAGE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return NOERROR;
}

_int CScene_Logo::Update_Scene(const _float & fTimeDelta)
{
	if (FAILED(Change_Scene(SCENE_STAGE)))
		return 0;


	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CScene_Logo::Render_Scene(void)
{
	lstrcpy(m_szThread, m_pLoading->Get_LodingFont());

	char	chThread[128] = "";

	WideCharToMultiByte(CP_ACP, 0, m_szThread, 128, chThread, 128, NULL, NULL);

	m_pD3dFont->DrawTextW(BACKSIZEX / 2, BACKSIZEY - 100.f, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), chThread);

	Engine::CScene::Render_Scene();
}

HRESULT CScene_Logo::Add_Logo_BGM()
{
	CSound_Manager::GetInstance()->Play_BGM(L"DungeonDefense_MainTheme.ogg", CSound_Manager::Channel_BGM, 1.f);

	return NOERROR;

}



HRESULT CScene_Logo::Ready_LogoComponent(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	// For.Texture_BackLogo
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/BackLogo_%d.tga", 18);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_LOGO, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;

	/* texture */
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/MainLogo/Main2.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_LOGO, L"Component_Texture_MainBackLogo", pComponent)))
		return E_FAIL;

	// For.Component_Shader_Default
	pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Default.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Shader_Default", pComponent)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CScene_Logo::Ready_D3dFont()
{
	m_pD3dFont = CGraphic_Device::GetInstance()->Get_Font();

	if (nullptr == m_pD3dFont)
	{
		MSG_BOX("Ready D3dFont Failed");
		return E_FAIL;
	}
	m_pD3dFont->AddRef();

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_Camera(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Camera
	pGameObject = CCamera_Dynamic::Create(m_pGraphicDev, pEye, pAt, pUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pCamera_Manager->Add_Camera(L"Dynamic_Camera", pGameObject)))
		return E_FAIL;

	m_pCamera_Manager->Change_Camera(L"Dynamic_Camera");

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	Engine::CGameObject*	pGameObject = nullptr;

	// For.Back_Logo
	pGameObject = CBack_Logo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_LOGO, pLayerTag, pGameObject)))
		return E_FAIL;

	// For.Back_Logo
	pGameObject = CMainBack_Logo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_LOGO, pLayerTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Logo::Change_Scene(SCENETYPE eSceneID)
{
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
	{

		if (false == m_pLoading->Get_Finish())
			return NOERROR;

		Engine::CScene*			pNewScene = nullptr;

		switch (eSceneID)
		{
		case SCENE_STAGE:
			pNewScene = CScene_Stage::Create(m_pGraphicDev);
			if (nullptr == pNewScene)
				return E_FAIL;
			break;
		}

		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return E_FAIL;
	}
	return NOERROR;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Logo *	pInstance = new CScene_Logo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("CScene_Logo Created Failed!!");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CScene_Logo::Free(void)
{
	if (FAILED(CScene::Release_Object(SCENE_LOGO)))	
		MSG_BOX("Scene_Logo Release Failed");		

	if(FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_LOGO)))
		MSG_BOX("Scene_Logo Release Failed");

	Safe_Release(m_pD3dFont);
	Safe_Release(m_pLoading);

	return Engine::CScene::Free();
}

