#include "stdafx.h"
#include "..\Headers\DragonKiller.h"

#include "Component_Manager.h"
#include "object_Manager.h"

#include "Layer.h"

#include "DragonArrow.h"
#include "Wave_Manager.h"

CDragonKiller::CDragonKiller(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pPlayerTrans(nullptr)
	, m_pWave_Manager(CWave_Manager::GetInstance())
{

}

HRESULT CDragonKiller::Ready_GameObject(_vec3* vPos, SCENETYPE eType)
{
	m_eType = eType;

	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_DegreeY(180.f);
	m_pTransformCom->Set_Position(*vPos);
	m_pTransformCom->Scaling(_vec3(0.002f, 0.002f, 0.002f));

	m_pBufferCom->Set_AnimationSet(0);

	return NOERROR;
}

_int CDragonKiller::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

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
		//Collision_ToPlayer();

		Move_Partern(fTimeDelta);
		Attack(fTimeDelta);
		CollTime(fTimeDelta);

		State_Update();
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

_int CDragonKiller::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CDragonKiller::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
#endif 

	if(m_bAnimation)
		m_pBufferCom->Play_Animation(m_fTimeDelta * 0.5f);

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

HRESULT CDragonKiller::Ready_Component()
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
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_DragonKiller");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	_vec3 vDegree = m_pPlayerTrans->Get_Rotation();
	vDegree.y += 180.f;
	
	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &vDegree);
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

HRESULT CDragonKiller::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

void CDragonKiller::Not_BuildMove()
{
	_vec3 vPos = m_pPlayerTrans->Get_Position();
	_vec3 vLook = m_pPlayerTrans->Get_Look();
	_vec3 vDegree = m_pPlayerTrans->Get_Rotation();
	vDegree.y += 180.f;

	vPos += *D3DXVec3Normalize(&vLook, &vLook);
	m_pTransformCom->Set_Position(vPos);
	m_pTransformCom->Set_Degree(vDegree);
}

void CDragonKiller::Set_Dgree(_vec3 * vDgree)
{
	vDgree->y += 180.f;
	m_pTransformCom->Set_Degree(*vDgree);
}

void CDragonKiller::Collision_ToPlayer()
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

void CDragonKiller::State_Update()
{
	if (DRAGON_STAND != m_eCurrState && DRAGON_END != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = DRAGON_STAND;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}

			if (DRAGON_ATTAKC == m_eCurrState)
				m_bAttack = false;
		}
	}
	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case DRAGON_STAND:
			m_pBufferCom->Set_AnimationSet(1);
			m_fDelayTime = 0.f;
			break;
		case DRAGON_ATTAKC:
			m_pBufferCom->Set_AnimationSet(0);
			m_bMotion = true;
 			m_bAttack = true;
			m_fDelayTime = -3.f;
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void CDragonKiller::Move_Partern(const _float & fTimeDelta)
{
	if (m_pWave_Manager->Get_WaveList()->empty())
		return;
	
	auto& iter = m_pWave_Manager->Get_WaveList()->begin();
	auto& iter_end = m_pWave_Manager->Get_WaveList()->end();

	for (; iter != iter_end; ++iter)
	{
		CTransform* pMonsterTrans = (CTransform*)(*iter)->Get_Component(L"Com_Transform");

		if (m_pTransformCom->Compute_Distance(&pMonsterTrans->Get_Position()) < 8.f)
		{
			m_vTargetPos = pMonsterTrans->Get_Position();
			_vec3 vLook = pMonsterTrans->Get_Position() - m_pTransformCom->Get_Position(); // 플레이어보는 룩
			_vec3 vMyLook = m_pTransformCom->Get_Look();
			_float fDgree = 0.f;
			m_pTransformCom->Get_LookDgreeY(fDgree, &-vLook, &vMyLook);
			if (abs(fDgree) > 5.f)
			{
				m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
				m_pBufferCom->Set_AnimationFirst();
			}
			else
			{
				if (true == m_bAttackBool)
				{
					if (m_bAttackStart)
					{
						m_eNewState = DRAGON_ATTAKC;
						m_bAttackBool = false;
						m_bAttacking = true;
					}
					else
						m_eNewState = DRAGON_STAND;
				}
				else
				{
					m_eNewState = DRAGON_STAND;
				}
			}
			break;
		}
		if (iter == iter_end)
		{
			m_vTargetPos = _vec3(0.f, 0.f, 0.f);
			m_eNewState = DRAGON_STAND;
		}

			//	m_pTargetMonster = pMonsterTrans;
			//	m_pMonster = (*iter);
			//	if (nullptr == m_pTargetMonster)
			//		m_pTargetMonster->AddRef();
			//	break;
			//}		
			//if (iter == iter_end)
			//{
			//	m_eNewState = DRAGON_STAND;
			//	m_pTargetMonster = nullptr;
			//	m_pMonster = nullptr;
			//	m_bAnimation = false;
			//	m_pBufferCom->Set_AnimationFirst();
			//	m_bStartAttack = false;
			//	return;
			//}
	}
	//if (nullptr != m_pTargetMonster)
	//{

	//	_vec3 vLook = m_pTargetMonster->Get_Position() - m_pTransformCom->Get_Position(); // 플레이어보는 룩
	//	_vec3 vMyLook = m_pTransformCom->Get_Look();

	//	_float fDgree = 0.f;

	//	m_pTransformCom->Get_LookDgreeY(fDgree, &-vLook, &vMyLook);

	//	if (abs(fDgree) > 5.f)
	//	{
	//		m_bAnimation = false;
	//		m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
	//		m_fStartTime = 1.1f;
	//		m_aniFirst = true;
	//		m_pBufferCom->Set_AnimationFirst();
	//		m_bStartAttack = false;
	//	}
	//	else
	//	{
	//		m_bAnimation = true;
	//		m_bStartAttack = true;
	//		if (m_bAttackCoolTime)
	//		{
	//			m_eNewState = DRAGON_ATTAKC;
	//			m_bStartAttack = true;
	//		}
	//		else
	//		{
	//			m_eNewState = DRAGON_STAND;
	//		}
	//	}
	//}
}

void CDragonKiller::Attack(const _float & fTimeDelta)
{
	if (m_bAttacking)
	{
		m_fAttackGo += fTimeDelta * 1.f;
		if (m_fAttackGo > 0.2f)
		{
			m_fAttackGo = 0.f;
			m_bAttackStart = false;
			m_bAttacking = false;

			_vec3 vPos = m_pTransformCom->Get_Position();
			_vec3 vLook = m_vTargetPos - vPos;
			_vec3 vRot = m_pTransformCom->Get_Rotation();

			m_pArrow = CDragonArrow::Create(m_pGraphicDev, &vPos, &vLook, &vRot, this, m_eType);
			if (nullptr == m_pArrow)
				return;
			CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Layer_DragonArrow", m_pArrow);
			
			m_aniFirst = false;
			m_bAttackCoolTime = false;
			m_aniFirst = false;

		}
	}
	//if (m_bStartAttack)
	//{
	//	if (m_bAttackCoolTime)
	//	{
	//		if (nullptr != m_pTargetMonster)
	//		{
	//			m_fMakeArrow += fTimeDelta * 1.f;
	//			if (m_fMakeArrow > 0.2f)
	//			{
	//				_vec3 vPos = m_pTransformCom->Get_Position();
	//				_vec3 vLook = m_pTargetMonster->Get_Position() - vPos;
	//				_vec3 vRot = m_pTransformCom->Get_Rotation();

	//				m_pArrow = CDragonArrow::Create(m_pGraphicDev, &vPos, &vLook, &vRot, this, m_eType);
	//				if (nullptr == m_pArrow)
	//					return;
	//				CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_DragonArrow", m_pArrow);
	//				m_fMakeArrow = 0.f;
	//				m_bAttackCoolTime = false;
	//				m_bStartAttack = false;
	//				m_aniFirst = false;
	//			}
	//		}
	//	}
	//}
}

void CDragonKiller::CollTime(const _float & fTimeDelta)
{
	if (m_aniFirst == false)
	{
		m_fStartTime = 5.f;
	}

	if (!m_bAttackCoolTime)
	{
		m_fAttackCoolTime += fTimeDelta * 1.f;
		if (m_fAttackCoolTime > m_fStartTime)
		{
			m_bAttackStart = true;
			m_bAttackCoolTime = true;
			m_fAttackCoolTime = 0.f;
			m_bStartAttack = true;
			m_bAttackBool = true;
			return;
		}
	}
}

CDragonKiller * CDragonKiller::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eType)
{
	CDragonKiller*	pInstance = new CDragonKiller(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPos, eType)))
	{
		MSG_BOX("CDragonKiller Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CDragonKiller::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPlayerTrans);

	//Engine::Safe_Release(m_pArrow);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
