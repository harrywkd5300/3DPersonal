#include "stdafx.h"
#include "MainApp.h"

#include "Component_Manager.h"
#include "object_manager.h"
#include "Management.h"

#include "Camera_Manager.h"

#include "Scene_Logo.h"

#include "Frame_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Wave_Manager.h"
#include "Sound_Manager.h"

USING(Engine)

CMainApp::CMainApp()
	: m_pGraphic_Device(Engine::CGraphic_Device::GetInstance())
	, m_pGraphicDev(nullptr)
	, m_pComponent_Manager(Engine::CComponent_Manager::GetInstance())
	, m_pManagement(Engine::CManagement::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
{
	m_pGraphic_Device->AddRef();
	m_pComponent_Manager->AddRef();
	m_pManagement->AddRef();
	m_pSound_Manager->AddRef();
}


HRESULT CMainApp::Ready_MainApp(void)
{
	if (FAILED(Ready_DefaultSetting(g_hWnd, CGraphic_Device::MODE_WIN, BACKSIZEX, BACKSIZEY)))
		return E_FAIL;

	if (FAILED(Ready_StaticComponent()))
		return E_FAIL;

	if (FAILED(Ready_Sound()))
		return E_FAIL;

	if (FAILED(Ready_StartScene()))
		return E_FAIL;
	
	srand(time(NULL));

	return NOERROR;
}

int CMainApp::Update_MainApp(const float & fTimeDelta)
{
	if (nullptr == m_pManagement)
		return -1;

	////ShowCursor(false);
	//m_fTimer += fTimeDelta;
	//++m_fFPS;

	m_pSound_Manager->Update_Sound();

	return m_pManagement->Update_Management(fTimeDelta);	
}

void CMainApp::Render_MainApp(void)
{
	if (nullptr == m_pGraphicDev
		|| nullptr == m_pManagement)
		return;

	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	m_pGraphicDev->BeginScene();

	// 그려야할객체들을 그린다.
	m_pManagement->Render_Management();

	//Render_Font();

	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
}


// 내 게임을 초기화하기위해 필요한 사전작업을 수행하낟.
HRESULT CMainApp::Ready_DefaultSetting(HWND hWnd, Engine::CGraphic_Device::WINMODE eMode, const _uint& iBackCX, const _uint& iBackCY)
{
	if (nullptr == m_pGraphic_Device
		|| nullptr == m_pManagement)
		goto except;

	if (FAILED(m_pGraphic_Device->Ready_GraphicDev(&m_pGraphicDev, hWnd, eMode, iBackCX, iBackCY)))
		goto except;

	// GARA
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if (FAILED(m_pManagement->Ready_Management(SCENE_END)))
		goto except;

	/*Add Font*/
	m_pFont = m_pGraphic_Device->Get_Font();
	if (nullptr == m_pFont)
		return E_FAIL;

	m_pFont->AddRef();

	return NOERROR;

except:
	MSG_BOX("Ready_DefaultSetting Failed");
	return E_FAIL;
}

HRESULT CMainApp::Ready_StaticComponent()
{
	Engine::CComponent*		pComponent = nullptr;

	//--For.Transform
	pComponent = Engine::CTransform::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Transform", pComponent)))
		return E_FAIL;

	// For.Picking
	pComponent = Engine::CPicking::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Picking", pComponent)))
		return E_FAIL;

	//--For.CTriangle_Color_Buffers
	pComponent = Engine::CTriangle_Color::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Buffer_TriCol", pComponent)))
		return E_FAIL;

	//--For.CRect_Color_Buffers
	pComponent = Engine::CRect_Color::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Buffer_RectCol", pComponent)))
		return E_FAIL;

	//--For.Rect_Texture
	pComponent = Engine::CRect_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Buffer_RectTex", pComponent)))
		return E_FAIL;

	//--For.RectUI_Texture
	pComponent = Engine::CRectUI_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Buffer_RectUITex", pComponent)))
		return E_FAIL;

	//--For.Renderer
	pComponent = Engine::CRenderer::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Ready_Component(SCENE_STATIC, L"Component_Renderer", pComponent)))
		return E_FAIL;
	m_pManagement->SetUp_Renderer((CRenderer*)pComponent);

	return NOERROR;
}

HRESULT CMainApp::Ready_StartScene(void)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	Engine::CScene*			pCurrentScene = nullptr;

	pCurrentScene = CScene_Logo::Create(m_pGraphicDev);
	if (nullptr == pCurrentScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_CurrentScene(pCurrentScene)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CMainApp::Ready_Sound()
{
	if (FAILED(m_pSound_Manager->Ready_Sound_Manager()))
	{
		MSG_BOX("Sound_Manager Load Failed!!");
		return E_FAIL;
	}
	return NOERROR;
}

void CMainApp::Render_Font()
{
	if (m_fTimer > 1.f)
	{
		sprintf_s(m_szFPS, "FPS : %d", _int(m_fFPS));

		m_fTimer = 0.f;
		m_fFPS = 0.f;
	}

	m_pFont->DrawTextW(10, 10, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), m_szFPS);
}

CMainApp * CMainApp::Create(void)
{
	CMainApp *	pInstance = new CMainApp();	
	
	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("CMainApp Created Failed!!");
		Engine::Safe_Release(pInstance);		
	}
		
	return pInstance;
}

unsigned long CMainApp::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pGraphic_Device);	
	Engine::Safe_Release(m_pComponent_Manager);
	Engine::Safe_Release(m_pSound_Manager);
	Engine::Safe_Release(m_pManagement);
	Engine::Safe_Release(m_pFont);

	_ulong	dwRefCnt = 0;	

	if (dwRefCnt = CManagement::GetInstance()->DestroyInstance())
		MSG_BOX("CManagement Released Failed");

	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CCamera_Manager Released Failed");

	if (dwRefCnt = CObject_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CObject_Manager Released Failed");

	if (dwRefCnt = CComponent_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CComponent_Manager Released Failed");

	if (dwRefCnt = CGraphic_Device::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphic_Device Released Failed");

	if (dwRefCnt = CFrame_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphic_Device Released Failed");

	if (dwRefCnt = CTimer_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphic_Device Released Failed");

	if (dwRefCnt = CInput_Device::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphic_Device Released Failed");

	if (dwRefCnt = CWave_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CWave_Manager Released Failed");

	if (dwRefCnt = CSound_Manager::GetInstance()->DestroyInstance())
		MSG_BOX("CSound_Manager Released Failed");

	return dwRefCnt;
}
