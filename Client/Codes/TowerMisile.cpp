#include "stdafx.h"
#include "..\Headers\TowerMisile.h"

#include "component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Monster.h"

#include "MisileTower.h"
#include "Fire_Explosion.h"
#include "Wave_Manager.h"

#include "Sound_Manager.h"

CTowerMisile::CTowerMisile(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pWave_Manager(CWave_Manager::GetInstance())
{
	m_pWave_Manager->AddRef();
}

HRESULT CTowerMisile::Ready_GameObject(_vec3 * vMakePos, CGameObject* pObject, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTower = pObject;

	m_vStartPos = *vMakePos;

	// Parent 있을경우
	m_pTransformCom->Set_Position(_vec3(0.f, 35.f, 35.f));
	m_pTransformCom->Scaling(_vec3(180.f, 180.f, 180.f));

	return NOERROR;
}

_int CTowerMisile::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		dynamic_cast<CMisileTower*>(m_pTower)->Set_DeadWeapon();

		return 1;
	}

	m_fTimeDelta = fTimeDelta;
	m_fTimeShader += fTimeDelta;

	if (m_bAttack)
	{
		Collision_ToTarget();

		_vec3 vPos = m_pTransformCom->Get_Position();

		m_pTransformCom->Move_ToDirection(m_vDir, 9.f, fTimeDelta);

		_vec3 vDistPos = m_pTransformCom->Get_Position() - m_vStartPos;

		m_fDist = D3DXVec3Length(&vDistPos);
	
		m_pTransformCom->Make_WorldMatrix();

		if (m_fDist > 7.f)
		{
			m_bIsDead = true;
		}
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

_int CTowerMisile::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;
	if (!m_bAttack)
	{
		m_pTransformCom->Make_WorldMatrix();
	}

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CTowerMisile::Render_GameObject(void)
{
	if (nullptr == m_pMaterialCom)
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

HRESULT CTowerMisile::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	//For.Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_FireBall");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 90.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.m_pMaterialCom
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Shader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CTowerMisile::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//_float	TimeDelta = m_fTimeDelta;
	// TimeDelta
	pEffect->SetFloat("g_fTimeDeltaU", m_fTimeShader * 1.7f);
	pEffect->SetFloat("g_fTimeDeltaV", m_fTimeShader * 1.7f);

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

	// For. Alpha
	pEffect->SetFloat("g_fAlpha", 1.f);

	//// For.Color
	pEffect->SetFloat("g_fColorG", 1.f);
	pEffect->SetFloat("g_fColorB", 1.f);
	pEffect->SetFloat("g_fColorR", 0.8f);
	pEffect->SetFloat("g_fColorA", 1.f);

	Safe_Release(pEffect);

	return NOERROR;
}

void CTowerMisile::Collision_ToTarget()
{
	if (m_pWave_Manager->Get_WaveList()->empty())
		return;

	auto& iter = m_pWave_Manager->Get_WaveList()->begin();
	auto& iter_end = m_pWave_Manager->Get_WaveList()->end();

	for (; iter != iter_end; ++iter)
	{
		if (dynamic_cast<CMonster*>((*iter))->Get_IsDead())
			continue;

		if (true == m_pColliderCom->Collision_AABB((CCollider*)(*iter)->Get_Component(L"Com_Collider")))
		{
			dynamic_cast<CMonster*>((*iter))->Set_Hit(50.f);

			_vec3 vPos = m_pTransformCom->Get_Position();
			_vec3 vScale = _vec3(1.8f, 1.8f, 1.8f);
			CGameObject* pObject = CFire_Explosion::Create(m_pGraphicDev, &vPos, &vScale, 14, m_eType);
			if (nullptr == pObject)
				return;
			CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Test_Explosion", pObject);
			CSound_Manager::GetInstance()->Play_Sound(L"Kobold_explode1.ogg", CSound_Manager::Channel_TowerAtk1, 1.f);
			m_bIsDead = true;
		}
	}
}

void CTowerMisile::Set_TowerAttack(_vec3 * vMakePos, _vec3* vDir)
{
	m_vDir = *vDir;

	vMakePos->y += 1.0f;
	m_pTransformCom->Set_Position(*vMakePos);

	m_vStartPos = *vMakePos;

	m_pTransformCom->Scaling(_vec3(1.f, 1.f, 1.f));

	_matrix matPostion;
	matPostion.Identity();
	m_pTransformCom->Parent(matPostion);

	m_bAttack = true;
}

CTowerMisile * CTowerMisile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 * vMakePos, CGameObject* pObject, SCENETYPE eType)
{
	CTowerMisile*	pInstance = new CTowerMisile(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vMakePos,  pObject, eType)))
	{
		MSG_BOX("CTowerMisile Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CTowerMisile::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pWave_Manager);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
