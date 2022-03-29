#include "stdafx.h"
#include "..\Headers\Monster.h"

#include "Component_Manager.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr), m_pRendererCom(nullptr), m_pBufferCom(nullptr)
	, m_pMaterialCom(nullptr), m_pColliderCom(nullptr), m_pShaderCom(nullptr), m_pNavigationCom(nullptr)
{
}

HRESULT CMonster::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return NOERROR;
}

_int CMonster::Update_GameObject(const _float & fTimeDelta)
{

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

_int CMonster::LastUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CMonster::Render_GameObject(void)
{
}

HRESULT CMonster::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Material");
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
	//pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom/*&_vec3(0.f, 0.f, 0.f)*/);
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	//m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	//For.Navigation
	//pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
	//m_pNavigationCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

void CMonster::Set_Hit(const _float & fAtk)
{
	m_fHp -= fAtk;
	m_bHit = true;
	if (m_fHp < 0)
		m_bMinusHP = true;
}

_ulong CMonster::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pNavigationCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
