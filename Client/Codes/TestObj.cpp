#include "stdafx.h"
#include "..\Headers\TestObj.h"


#include "Component_Manager.h"
#include "object_Manager.h"

#include "object_Manager.h"
#include "Component_manager.h"
#include "layer.h"
#include "Player.h"
#include "Arrow.h"

CTestObj::CTestObj(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

HRESULT CTestObj::Ready_GameObject(const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot)
{
	m_pObjectData = new OBJECT;
	ZeroMemory(m_pObjectData, sizeof(OBJECT));

	lstrcpy(m_pObjectData->FullPath, FullPath);
	lstrcpy(m_pObjectData->FileName, FileName);
	lstrcpy(m_pObjectData->ComName, ComPath);

	if (FAILED(Ready_MeshComponent(ComPath)))
		return E_FAIL;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(vMakeScale);
	m_pTransformCom->Set_Position(vMakePos);
	m_pTransformCom->Set_Degree(vMakeRot);

	return NOERROR;
}

_int CTestObj::Update_GameObject(const _float & fTimeDelta)
{

	const _vec3 vPosition = m_pTransformCom->Get_Position();

	if (nullptr != m_pFrustum)
		m_isDraw = m_pFrustum->isIn_Frustum(&vPosition, 2.f);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if(m_isDraw)
		Collision_ToObject();

	if (nullptr != m_pRendererCom && true == m_isDraw)	
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CTestObj::LastUpdate_GameObject(const _float & fTimeDelta)
{

	m_pTransformCom->Make_WorldMatrix();

	CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CTestObj::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	if (nullptr == m_pMaterialCom)
		return;

	m_pTransformCom->SetUp_OnGraphicDev();
	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
#endif 

	m_pBufferCom->Render_Mesh();
}

HRESULT CTestObj::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Frustum
	pComponent = m_pFrustum = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Frustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Frustum", pComponent));
	m_pFrustum->AddRef();

	return NOERROR;
}

HRESULT CTestObj::Ready_MeshComponent(const _tchar * ComPath)
{
	CComponent*			pComponent = nullptr;

	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, ComPath);
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

void CTestObj::Move_OnTerrain()
{
	if (nullptr == m_pTransformCom)
		return;

	if (nullptr == m_pTargetBufferCom)
	{
		m_pTargetBufferCom = (CVIBuffer*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_BackGround", 0, L"Com_Buffer");
		m_pTargetBufferCom->AddRef();
	}
	m_pTransformCom->Move_OnBuffer(m_pTargetBufferCom);
}

void CTestObj::Collision_ToObject(void)
{
	CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Player", 0, L"Com_Collider");
	if (nullptr == pCollider_Player)
		return;

	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	_vec3 vPos = m_pPlayerTrans->Get_Position();

	if (m_pTransformCom->Compute_Distance(&vPos) < 5.f)
	{
		m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player);

		CLayer*	pLayer = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Arrow");
		if (nullptr == pLayer)
			return;

		if (pLayer->GetObjList()->empty())
			return;

		auto& iter = pLayer->GetObjList()->front();

		CComponent*		pCollider_Weapon = dynamic_cast<CArrow*>(iter)->Get_Component(L"Com_Collider");
		if (nullptr == pCollider_Weapon)
			return;
		m_pColliderCom->Collision_AABB((CCollider*)pCollider_Weapon);
	}

}

CTestObj * CTestObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot)
{
	CTestObj*	pInstance = new CTestObj(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(FullPath, FileName, ComPath, vMakeScale, vMakePos, vMakeRot)))
	{
		MSG_BOX("CTestObj Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CTestObj::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);

	Engine::Safe_Release(m_pTargetBufferCom);
	Engine::Safe_Release(m_pPlayerTrans);
	Engine::Safe_Release(m_pFrustum);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
