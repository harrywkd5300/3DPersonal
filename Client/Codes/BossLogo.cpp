#include "stdafx.h"
#include "..\Headers\BossLogo.h"

#include "Component_manager.h"
#include "Camera_Dynamic.h"
#include "Input_Device.h"
#include "Scene_Boss.h"
#include "Management.h"

#include "object_Manager.h"
#include "Layer.h"

#include "Graphic_Device.h"
#include "Loading.h"
#include "Back_Logo.h"

CBossLogo::CBossLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

HRESULT CBossLogo::Ready_Scene(void)
{
	if (FAILED(Set_UI()))
		return E_FAIL;

	if (FAILED(Ready_D3dFont()))
		return E_FAIL;

	if (FAILED(Ready_LogoComponent()))
		return E_FAIL;

	if (FAILED(Change_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphicDev, SCENE_BOSS);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return NOERROR;
}

_int CBossLogo::Update_Scene(const _float & fTimeDelta)
{
	if (FAILED(Change_Scene(SCENE_BOSS)))
		return 0;

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CBossLogo::Render_Scene(void)
{
	lstrcpy(m_szThread, m_pLoading->Get_LodingFont());

	char	chThread[128] = "";

	WideCharToMultiByte(CP_ACP, 0, m_szThread, 128, chThread, 128, NULL, NULL);

	m_pD3dFont->DrawTextW(BACKSIZEX / 2, BACKSIZEY - 100.f, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), chThread);

	Engine::CScene::Render_Scene();
}

HRESULT CBossLogo::Ready_LogoComponent(void)
{
	// For.Logo_Scene Component
	Engine::CComponent*			pComponent = nullptr;

	// For.Texture_BackLogo
	pComponent = Engine::CTexture::Create(m_pGraphicDev, Engine::CTexture::TYPE_NORMAL, L"../Bin/Resources/Textures/BackLogo/BackLogo_%d.tga", 18);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_LOGO, L"Component_Texture_BackLogo", pComponent)))
		return E_FAIL;


	//// For.Component_Shader_Default
	//pComponent = CShader::Create(m_pGraphicDev, L"../Bin/ShadersFiles/Shader_Default.fx");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"Component_Shader_Default", pComponent)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CBossLogo::Ready_D3dFont()
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

HRESULT CBossLogo::Change_Camera()
{
	m_pCamera_Manager->Change_Camera(L"Dynamic_Camera");

	return NOERROR;
}

HRESULT CBossLogo::Ready_Layer_BackGround(const _tchar * pLayerTag)
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

	return NOERROR;
}

HRESULT CBossLogo::Set_UI()
{
	auto& iter     = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_UI")->GetObjList()->begin();
	auto& iter_end = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_UI")->GetObjList()->end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Set_isUpdate(true);
	}

	return NOERROR;
}

HRESULT CBossLogo::Change_Scene(SCENETYPE eSceneID)
{
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
	{

		if (false == m_pLoading->Get_Finish())
			return NOERROR;

		Engine::CScene*			pNewScene = nullptr;

		switch (eSceneID)
		{
		case SCENE_BOSS:
			pNewScene = CScene_Boss::Create(m_pGraphicDev);
			if (nullptr == pNewScene)
				return E_FAIL;
			break;
		}

		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return E_FAIL;
	}
	return NOERROR;
}

CBossLogo * CBossLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossLogo *	pInstance = new CBossLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("CScene_Logo Created Failed!!");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CBossLogo::Free(void)
{
	if (FAILED(CScene::Release_Object(SCENE_LOGO)))
		MSG_BOX("Scene_Logo Release Failed");

	if (FAILED(Engine::CComponent_Manager::GetInstance()->Release_Component(SCENE_LOGO)))
		MSG_BOX("Scene_Logo Release Failed");

	Safe_Release(m_pD3dFont);
	Safe_Release(m_pLoading);

	return Engine::CScene::Free();
}
