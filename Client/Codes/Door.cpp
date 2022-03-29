#include "stdafx.h"
#include "..\Headers\Door.h"

#include "component_Manager.h"
#include "object_Manager.h"

#include "management.h"
#include "Scene_Stage.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pColliderCom(nullptr), m_pMaterialCom(nullptr)
	, m_pRendererCom(nullptr), m_pShaderCom(nullptr), m_pTransformCom(nullptr)
	, m_bLeft(true), m_vRota(0.f, 0.f, 0.f)
{

}

HRESULT CDoor::Ready_GameObject(_bool Left, _vec3* vPos, _vec3* vRot)
{
	m_bLeft = Left;
	m_vRota = *vRot;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Position(*vPos);
	m_pTransformCom->Set_Degree(m_vRota);
	m_pTransformCom->Scaling(_vec3(0.0043f, 0.0043f, 0.0043f));

	return NOERROR;
}

_int CDoor::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if(m_bOpen)
		Open_Door(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CDoor::LastUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CDoor::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
#endif 

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();


	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CDoor::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	// For.LightInfo
	D3DLIGHT9			LightInfo;
	m_pGraphicDev->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLight_Direction", &_vec4(LightInfo.Direction, 0.f));
	pEffect->SetVector("g_vLight_Diffuse", (_vec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLight_Ambient", (_vec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLight_Specular", (_vec4*)&LightInfo.Specular);

	// For.MaterialInfo

	pEffect->SetVector("g_vMaterial_Diffuse", &_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->SetVector("g_vMaterial_Ambient", &_vec4(0.3f, 0.3f, 0.3f, 1.f));
	pEffect->SetVector("g_vMaterial_Specular", &_vec4(1.f, 1.f, 1.f, 1.f));

	// For.Camera	
	D3DXMatrixInverse(&matView, nullptr, &matView);

	pEffect->SetVector("g_vCamera_Position", (_vec4*)&matView.m[3][0]);
	pEffect->SetFloat("g_fPower", 20.0f);

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CDoor::Ready_Component()
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
	
	if (m_bLeft)
	{
		// For.Buffers
		pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_LeftDoor");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
		m_pBufferCom->AddRef();
	}
	else
	{
		// For.Buffers
		pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_RightDoor");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
		m_pBufferCom->AddRef();
	}

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &m_vRota);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

void CDoor::Open_Door(const _float& fTimeDelta)
{
	if (m_pTransformCom->Get_DegreeY() > 360 || m_pTransformCom->Get_DegreeY() < 180)
	{
		Collision_ToPlayer();
		return;
	}
	m_pTransformCom->Rotation_Y(m_vOpen.y * fTimeDelta * 5.f);
}

void CDoor::Collision_ToPlayer()
{

	CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
	if (nullptr == pCollider_Player)
		return;

	if(m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player))
	{
		CScene* pScene = CManagement::GetInstance()->Get_CurrentScene();
		dynamic_cast<CScene_Stage*>(pScene)->Change_BoolScene();
	}
}

CDoor * CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool Left,_vec3* vPos, _vec3* vRot)
{
	CDoor *	pInstance = new CDoor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(Left, vPos, vRot)))
	{
		MSG_BOX("CDoor Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CDoor::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
