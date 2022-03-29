#include "stdafx.h"
#include "PickObj.h"

#include "component_Manager.h"

CPickObj::CPickObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)

{
}

HRESULT CPickObj::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	//m_pTransformCom->Scaling(_vec3(0.5f, 0.5f, 0.5f));

	return NOERROR;
}

_int CPickObj::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CPickObj::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return 0;
}

void CPickObj::Render_GameObject(void)
{
	if (nullptr != m_pBufferCom)
		return;

	m_pBufferCom->Render_Buffer();

}

HRESULT CPickObj::Ready_Component(void)
{
	CComponent*			pComponent = nullptr;
	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.BufferCom
	pComponent = m_pBufferCom = (Engine::CCube_Texture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Buffer_Cube");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();


	return NOERROR;
}

void CPickObj::Set_Position(const _vec3 & vPos)
{
	m_pTransformCom->Set_Position(vPos);
}

CPickObj * CPickObj::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPickObj*	pInstance = new CPickObj(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPickObj Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CPickObj::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
