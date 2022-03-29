#include "stdafx.h"
#include "..\Headers\LightningBall.h"

#include "component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "LightningTower.h"
#include "Wave_Manager.h"

#include "Monster.h"

CLighningBall::CLighningBall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_pBufferCom(nullptr), m_pWave_Manager(CWave_Manager::GetInstance())
{
}

HRESULT CLighningBall::Ready_GameObject(_vec3 * vMakePos, CGameObject* pObject, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTower = pObject;

	vMakePos->y += 1.4f;
	m_pTransformCom->Set_Position(*vMakePos);

	m_vStartPos = *vMakePos;

	m_pTransformCom->Scaling(m_vFirstScale);

	return NOERROR;
}

_int CLighningBall::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		dynamic_cast<CLightningTower*>(m_pTower)->Set_DeadWeapon();
		return 1;
	}

	m_fTimeDelta = fTimeDelta;
	m_fTimeShader += fTimeDelta;

	if (m_bBigMake)
	{
		if (m_bEndMake)
		{
			m_vFirstScale.x += 0.001f * fTimeDelta;
			m_vFirstScale.y += 0.001f * fTimeDelta;
			m_vFirstScale.z += 0.001f * fTimeDelta;

			if (m_vFirstScale.x < 0.005f)
				m_pTransformCom->Scaling(m_vFirstScale);
			else
				m_bEndMake = false;
		}
		else if (m_bAttakMove)
		{
			if (m_bCheck)
			{
				m_bCheck = false;
				m_vEndPos.y += 1.6f;
				m_vDir = m_vEndPos - m_pTransformCom->Get_Position();
			}

			m_pTransformCom->Move_ToDirection(m_vDir, 8.f, fTimeDelta);

			_vec3 vDistPos = m_pTransformCom->Get_Position() - m_vStartPos;

			m_fDist = D3DXVec3Length(&vDistPos);
		}
		else
		{
			dynamic_cast<CLightningTower*>(m_pTower)->Set_Attack(true);
			m_bAttakMove = true;
		}
	}

	if (m_fDist > 10.f)
	{
		m_bBigMake = false;

		_vec3 vCurScale = m_pTransformCom->Get_Scale();

		vCurScale.x += 0.1f * fTimeDelta;
		vCurScale.y += 0.1f * fTimeDelta;
		vCurScale.z += 0.1f * fTimeDelta;

		Collision_ToTarget();

		if (vCurScale.x < 0.07f)
			m_pTransformCom->Scaling(vCurScale);
		else
		{
			m_fDeadTime += fTimeDelta * 1.f;
			if (m_fDeadTime > 1.5f)
			{
				m_bIsDead = true;
				dynamic_cast<CLightningTower*>(m_pTower)->Set_Attack(false);
				dynamic_cast<CLightningTower*>(m_pTower)->Set_AttackCoolTime(true);
			}
		}
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

_int CLighningBall::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CLighningBall::Render_GameObject(void)
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

HRESULT CLighningBall::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	//For.Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_EnergyBall");
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

HRESULT CLighningBall::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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
	//pEffect->SetFloat("g_fColorG", 20.f);
	//pEffect->SetFloat("g_fColorB", 20.f);
	//pEffect->SetFloat("g_fColorR", 20.f);
	//pEffect->SetFloat("g_fColorA", 1.f);

	Safe_Release(pEffect);

	return NOERROR;
}

void CLighningBall::Collision_ToTarget()
{
	if (m_pWave_Manager->Get_WaveList()->empty())
		return;

	auto& iter	   = m_pWave_Manager->Get_WaveList()->begin();
	auto& iter_end = m_pWave_Manager->Get_WaveList()->end();

	for (; iter != iter_end; ++iter)
	{
		if (dynamic_cast<CMonster*>((*iter))->Get_IsDead())
			continue;

		CTransform*		pCollider_Monster = (CTransform*)(*iter)->Get_Component(L"Com_Transform");
		if (nullptr == pCollider_Monster)
			return;
		if (m_pTransformCom->Compute_Distance(&pCollider_Monster->Get_Position()) < 5.f)
		{
			CComponent*		pCollider = (*iter)->Get_Component(L"Com_Collider");
			if (true == m_pColliderCom->Collision_AABB((CCollider*)pCollider))
			{
				dynamic_cast<CMonster*>((*iter))->Set_Hit(1.f);
			}
		}
	}
}

CLighningBall * CLighningBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 * vMakePos, CGameObject* pObject, SCENETYPE eType)
{
	CLighningBall*	pInstance = new CLighningBall(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vMakePos,  pObject, eType)))
	{
		MSG_BOX("CLighningBall Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CLighningBall::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
