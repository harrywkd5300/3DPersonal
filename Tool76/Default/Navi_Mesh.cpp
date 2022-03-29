#include "stdafx.h"
#include "Navi_Mesh.h"

#include "component_Manager.h"

CNavi_Mesh::CNavi_Mesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pNavigation(nullptr)
{
}

HRESULT CNavi_Mesh::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return NOERROR;
}

_int CNavi_Mesh::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CNavi_Mesh::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return 0;
}

void CNavi_Mesh::Render_GameObject(void)
{
	if (nullptr == m_pNavigation)
		return;


	m_pNavigation->Render_Navigation();
}

HRESULT CNavi_Mesh::Ready_Component(void)
{
	CComponent*			pComponent = nullptr;
	// For.Transform
	pComponent = m_pNavigation = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Navigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pNavigation->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	

	return NOERROR;
}

void CNavi_Mesh::Make_NaviMesh(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC)
{
	m_pNavigation->Add_Cell(pPointA, pPointB, pPointC);
}

void CNavi_Mesh::Reserve_Navigation(const _ulong & Size)
{
	m_pNavigation->Reserve_Navigation(Size);
}

vector<CCell*>* CNavi_Mesh::Get_VecCell()
{
	return m_pNavigation->Get_VecCell();
}

void CNavi_Mesh::All_Clear()
{
	m_pNavigation->All_Clear();
}

void CNavi_Mesh::Select_Delete(const _int& iCellIdx)
{
	m_pNavigation->Select_Clear(iCellIdx);
}

_uint CNavi_Mesh::Picking_Navigation(const _vec3 * vPickPos)
{
	return m_pNavigation->Picking_Navigation(vPickPos);
}

_bool CNavi_Mesh::Picking_Dist_Short(const _vec3 * vPickPos, _vec3 & vGetPos)
{
	return m_pNavigation->Dist_Short(vPickPos, vGetPos);
}

void CNavi_Mesh::Set_PoInt(const _int iIdx, const _vec3 & vClickPos, const _vec3 vChangePos)
{
	m_pNavigation->Set_SelCellPoint(iIdx, vClickPos, vChangePos);
}

CNavi_Mesh * CNavi_Mesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNavi_Mesh*	pInstance = new CNavi_Mesh(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CNavi_Mesh Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CNavi_Mesh::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pNavigation);
	Engine::Safe_Release(m_pRendererCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
