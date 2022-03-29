#include "stdafx.h"
#include "DynamicObject.h"

#include "component_Manager.h"

CDynamicObject::CDynamicObject(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

HRESULT CDynamicObject::Ready_GameObject(TCHAR ComPath[128], const _vec3& vMakeScale)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_MeshComponent(ComPath)))
		return E_FAIL;

	m_pTransformCom->Scaling(vMakeScale);

	return NOERROR;
}

_int CDynamicObject::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	m_pTransformCom->Make_WorldMatrix();

	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CDynamicObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return 0;
}

void CDynamicObject::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x0f);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTransformCom->SetUp_OnGraphicDev();

	m_pBufferCom->Render_Mesh();
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CDynamicObject::Ready_Component(void)
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

	return NOERROR;
}

HRESULT CDynamicObject::Ready_MeshComponent(TCHAR ComPath[128])
{
	CComponent*			pComponent = nullptr;

	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComPath);
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

CDynamicObject * CDynamicObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, TCHAR ComPath[128], const _vec3& vMakeScale)
{
	CDynamicObject*		pInstance = new CDynamicObject(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(ComPath, vMakeScale)))
	{
		MSG_BOX("CDynamicObject Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CDynamicObject::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
