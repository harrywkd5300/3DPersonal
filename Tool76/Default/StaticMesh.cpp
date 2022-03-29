#include "stdafx.h"
#include "StaticMesh.h"

#include "component_Manager.h"

CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_bViewCollBox(true), m_bViewWire(false)
{

}

HRESULT CStaticMesh::Ready_GameObject(const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot)
{
	m_pObjectData = new OBJECT;
	ZeroMemory(m_pObjectData, sizeof(OBJECT));

	lstrcpy(m_pObjectData->FullPath, FullPath);
	lstrcpy(m_pObjectData->FileName, FileName);
	lstrcpy(m_pObjectData->ComName, ComPath);

	m_vRota = vMakeRot;

	if (FAILED(Ready_MeshComponent(ComPath)))
		return E_FAIL;
	if (FAILED(Ready_Component()))
		return E_FAIL;
	
	m_pTransformCom->Scaling(vMakeScale);
	m_pTransformCom->Set_Position(vMakePos);
	m_pTransformCom->Set_Degree(vMakeRot);

	return NOERROR;
}

_int CStaticMesh::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	m_pTransformCom->Make_WorldMatrix();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	const _vec3 vPosition = m_pTransformCom->Get_Position();

	//if (nullptr != m_pFrustum)
	//	m_isDraw = m_pFrustum->isIn_Frustum(&vPosition, 2.f);

	if (nullptr != m_pRendererCom && true == m_isDraw)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CStaticMesh::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return 0;
}

void CStaticMesh::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	if (m_bViewWire)
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x0f);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pTransformCom->SetUp_OnGraphicDev();
	m_pMaterialCom->SetUp_OnGraphicDev();

	if (m_bViewCollBox)
	{
#ifdef _DEBUG
		m_pColliderCom->Render_Buffer();
#endif 
	}

	m_pBufferCom->Render_Mesh();
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CStaticMesh::Ready_Component(void)
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

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &m_vRota);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Frustum
	pComponent = m_pFrustum = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Frustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Frustum", pComponent));
	m_pFrustum->AddRef();

	// For.m_pMaterialCom
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();


	return NOERROR;
}

HRESULT CStaticMesh::Ready_MeshComponent(const _tchar* ComPath)
{
	CComponent*			pComponent = nullptr;

	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComPath);
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

LPD3DXMESH* CStaticMesh::Get_Mesh()
{
	return m_pBufferCom->Get_Mesh();
}

void CStaticMesh::Get_WorldMatrix(_matrix * matWorld)
{
	m_pTransformCom->Get_WorldMatrix(matWorld);
}

_bool CStaticMesh::Get_RedCollBox()
{
	return m_pColliderCom->Get_IsColl();
}

OBJECT*& CStaticMesh::Get_ObjectData()
{
	m_pObjectData->vPostion = m_pTransformCom->Get_Position();
	m_pObjectData->vScale = m_pTransformCom->Get_Scale();
	m_pObjectData->vRotation = m_pTransformCom->Get_Rotation();

	return m_pObjectData;
}

void CStaticMesh::Set_PickObjectCollider(_bool Coll)
{
	m_pColliderCom->Set_Coll(Coll);
}

CStaticMesh * CStaticMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot)
{
	CStaticMesh*	pInstance = new CStaticMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(FullPath, FileName, ComPath, vMakeScale, vMakePos, vMakeRot)))
	{
		MSG_BOX("CStaticMesh Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CStaticMesh::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pFrustum);
	Engine::Safe_Release(m_pMaterialCom);

	Engine::Safe_Delete(m_pObjectData);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
