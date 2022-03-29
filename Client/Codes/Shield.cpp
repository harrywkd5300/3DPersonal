#include "stdafx.h"
#include "..\Headers\Shield.h"

#include "Component_Manager.h"
#include "object_Manager.h"

#include "Monster.h"

CShield::CShield(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pPlayerTrans(nullptr)
{

}

HRESULT CShield::Ready_GameObject(_vec3* vPos, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Position(*vPos);
	m_pTransformCom->Scaling(_vec3(0.005f, 0.005f, 0.005f));

	m_pBufferCom->Set_AnimationSet(0);

	return NOERROR;
}

_int CShield::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		Dead_To_MonsterTalk();
		return 1;
	}
	m_fTimeDelta = fTimeDelta;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bNotBuild)
	{
		m_iPass = 2;
		m_fAlpha = 0.6f;
		Not_BuildMove();
	}
	else
	{
		m_iPass = 1;
		m_fAlpha = 1.f;
		Collision_ToPlayer();
	}

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
	{
		if (m_bNotBuild)
			m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);
		else
			m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	}
	return 0;
}

_int CShield::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;



	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CShield::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
#endif 

	m_pBufferCom->Play_Animation(m_fTimeDelta);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();


	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(m_iPass);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CShield::Ready_Component()
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

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_Shield");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;

	_vec3 vMin = *m_pBufferCom->Get_Min();
	_vec3 vMax = *m_pBufferCom->Get_Max();

	vMin.x *= 1.f;
	vMin.z *= 3.f;

	vMax.x *= 1.f;
	vMax.z *= 3.f;


	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CShield::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

	// For.Alpha
	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	Safe_Release(pEffect);

	return NOERROR;
}

void CShield::Not_BuildMove()
{
	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	_vec3 vPos = m_pPlayerTrans->Get_Position();
	_vec3 vLook = m_pPlayerTrans->Get_Look();
	_vec3 vDegree = m_pPlayerTrans->Get_Rotation();
	//vDegree.y += 180.f;

	vPos += *D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom->Set_Position(vPos);
	m_pTransformCom->Set_Degree(vDegree);
}

void CShield::Collision_ToPlayer()
{
	if (nullptr == m_pPlayerTrans)
		return;

	_vec3 vPos = m_pPlayerTrans->Get_Position();

	if (m_pTransformCom->Compute_Distance(&vPos) < 5.f)
	{
		CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Player", 0, L"Com_Collider");
		if (nullptr == pCollider_Player)
			return;

		m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player);
	}
}

void CShield::Dead_To_MonsterTalk()
{
	auto iter = m_Monster.begin();
	auto iter_end = m_Monster.end();

	for (; iter != iter_end; ++iter)
	{
		dynamic_cast<CMonster*>(*iter)->Set_DeadShield();
	}
}

void CShield::Set_Dgree(_vec3 * vDgree)
{
	m_pTransformCom->Set_Degree(*vDgree);
}

void CShield::Set_ListObj(CGameObject * pObject)
{
	auto iter = m_Monster.begin();
	auto iter_end = m_Monster.end();

	for (; iter != iter_end; ++iter)
	{
		if((*iter) == pObject)
			return;
	}
	m_Monster.push_back(pObject);
}


CShield * CShield::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eType)
{
	CShield*	pInstance = new CShield(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPos, eType)))
	{
		MSG_BOX("CShield Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CShield::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPlayerTrans);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
