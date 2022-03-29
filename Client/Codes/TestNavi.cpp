#include "stdafx.h"
#include "..\Headers\TestNavi.h"

#include "Component_Manager.h"

CTestNavi::CTestNavi(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pNavigationCom(nullptr)
{
}

HRESULT CTestNavi::Ready_GameObject(SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	return NOERROR;
}

_int CTestNavi::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CTestNavi::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return 0;
}

void CTestNavi::Render_GameObject(void)
{
	m_pNavigationCom->Render_Navigation();
}

HRESULT CTestNavi::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	if (SCENE_STAGE == m_eType)
	{
		// For.Material
		pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Navigation_Santa");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigation", pComponent));
		m_pNavigationCom->AddRef();
	}
	else
	{
		// For.Material
		pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Navigation");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigation", pComponent));
		m_pNavigationCom->AddRef();
	}

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	return NOERROR;
}

CTestNavi * CTestNavi::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType)
{
	CTestNavi*	pInstance = new CTestNavi(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(eType)))
	{
		MSG_BOX("CTestNavi Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CTestNavi::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pNavigationCom);
	Engine::Safe_Release(m_pRendererCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
