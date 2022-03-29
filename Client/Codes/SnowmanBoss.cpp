#include "stdafx.h"
#include "..\Headers\SnowmanBoss.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Player.h"
#include "BaseObj.h"

SnowmanBoss::SnowmanBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_fTimeDelta(0.f), m_fDelayTime(0.f), m_fAniTime(1.f), m_bMotion(false)
{
}

HRESULT SnowmanBoss::Ready_GameObject(SCENETYPE eType)
{
	if (FAILED(Ready_Mesh()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_GameObject()))
		return E_FAIL;

	e_CurType = eType;

	// Stage
	m_pTransformCom->Set_Position(_vec3(40.f, 0.f, 64.f));

	m_pTransformCom->Scaling(_vec3(0.022f, 0.022f, 0.022f));

	m_pTransformCom->Rotation_Y(90.f);

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	m_eNewState = SNOWMAN_IDLE;

	m_fHp = 1500.f;

	return NOERROR;
}

_int SnowmanBoss::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		return 1;
	}

	if (!m_bFirst)
		return 0;

	m_fTimeDelta = fTimeDelta;

	if (!m_bOpenBoss)
	{
		if (m_fHp > 0)
		{
			m_iPass = 1;

			Move_Partern(fTimeDelta);

			if (m_bAttack)
				Attack(fTimeDelta);
			else
			{
				m_fHitTime += fTimeDelta * 1.f;
				if (m_fHitTime > 1.f)
				{
					m_fHitTime = 0.f;
					Collision_ToPlayer();
				}
			}

			CollTime(fTimeDelta);
		}
		else
		{
			CWave_Manager::GetInstance()->MonsterDead(this);
			if (m_bMinusHP)
			{
				m_iPass = 2;
				m_bIsDead = true;
			}
			m_eNewState = SNOWMAN_DEATH;
		}
	}
	else
	{
		m_iPass = 2;
		m_fAlpha += 0.1f * fTimeDelta;
	}

	CMonster::Update_GameObject(fTimeDelta);

	State_Update();

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int SnowmanBoss::LastUpdate_GameObject(const _float & fTimeDelta)
{

	CMonster::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void SnowmanBoss::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
	m_pMyCollider->Render_Buffer();
#endif 

	if (!m_bMinusHP || m_bOpenBoss)
		m_pBufferCom->Play_Animation(m_fTimeDelta, m_fAniTime);

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

HRESULT SnowmanBoss::Ready_Mesh()
{
	CComponent*			pComponent = nullptr;

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Mesh_SnowMansBoss");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	//For.Navigation
	pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
	m_pNavigationCom->AddRef();

	return NOERROR;
}

HRESULT SnowmanBoss::Ready_Collider()
{
	CComponent*			pComponent = nullptr;

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Collider
	pComponent = m_pMyCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_SnowCol");
	if (nullptr == pComponent)
		return E_FAIL;

	_vec3 vMin = *m_pBufferCom->Get_Min();
	vMin *= 1.5f;
	_vec3 vMax = *m_pBufferCom->Get_Max();
	vMax *= 1.5f;

	m_pMyCollider->Set_ColliderSetting(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MyCollider", pComponent));
	m_pMyCollider->AddRef();

	return NOERROR;
}

void SnowmanBoss::State_Update()
{
	if (SNOWMAN_IDLE != m_eCurrState && SNOWMAN_FORWARD != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = SNOWMAN_IDLE;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}

			if (SNOWMAN_ATTACK2 == m_eCurrState)
			{
				m_bAttack = false;
				m_bSelCol = true;
				m_bAttackCoolTime2 = false;
			}

			if (SNOWMAN_DEATH == m_eCurrState)
			{
				m_bMinusHP = true;
			}
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case SNOWMAN_IDLE:
			m_pBufferCom->Set_AnimationSet(3);
			m_fDelayTime = 0.6f;
			break;
		case SNOWMAN_FORWARD:
			m_pBufferCom->Set_AnimationSet(2);
			break;
		case SNOWMAN_DEATH:
			m_pBufferCom->Set_AnimationSet(4);
			m_bMotion = true;
			break;
		case SNOWMAN_ATTACK1:
			m_pBufferCom->Set_AnimationSet(6);
			m_fDelayTime = 0.1f;
			m_fAniTime = 1.f;
			m_bMotion = true;
			m_vJumpDir = m_pTargetPlayer->Get_Position() - m_pTransformCom->Get_Position();
			break;
		case SNOWMAN_ATTACK2:
			m_pBufferCom->Set_AnimationSet(7);
			m_fDelayTime = 0.3f;
			m_bAttack = true;
			m_bMotion = true;
			m_bAttackCoolTime2 = true;
			m_eAtkState = SNOWMANATK_ATK;
			m_bSelCol = false;
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void SnowmanBoss::Move_Partern(const _float & fTimeDelta)
{
	if (nullptr == m_pTargetPlayer)
	{
		m_pTargetPlayer = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pTargetPlayer->AddRef();
	}

	_vec3 vPos = m_pTargetPlayer->Get_Position();

	_float fDist = m_pTransformCom->Compute_Distance(&vPos);

	if (25.f > fDist)
	{
		_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);

		if (abs(fDgree) > 5.f)
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta);
		else
		{
			if (m_bJumpAttackCoolTime && 8.f < fDist)
			{
				if (!m_bPattern0)
				{
					m_eNewState = SNOWMAN_ATTACK1;
					m_fJumpAttackStartTime += fTimeDelta * 1.f;
					if (m_fJumpAttackStartTime > 1.6f)
					{
						m_bAttack = true;
						m_eAtkState = SNOWMAATK_JUMP;
						// 문제점 : 점프하는 패턴 캔슬되면 안움직임 해결하기
					}
				}
			}
			else if (2.2f > fDist && m_bAttackCoolTime2)
			{
				if (m_bAttackCoolTime2)
				{
					if (!m_bPattern2)
					{
						m_eNewState = SNOWMAN_ATTACK2;
					}
				}
			}
			else if(2.2f < fDist)
			{
				if (!m_pColliderCom->Get_IsColl())
				{
					m_eNewState = SNOWMAN_FORWARD;
					m_pTransformCom->Move_ToDirection(vLook, 6.f, fTimeDelta);
				}
			}
			else
			{
				m_eNewState = SNOWMAN_IDLE;
			}
		}
	}
}

void SnowmanBoss::Attack(const _float & fTimeDelta)
{
	switch (m_eAtkState)
	{
	case SNOWMAATK_JUMP:
	{
		m_bPattern0 = true;
		float height = (m_fJumpTime * m_fJumpTime * (-9.8f) / 2) + (m_fJumpTime * (m_fJumpPower - 1.f));
		m_pTransformCom->Set_PositionY(m_fPosY + height);
		//점프시간을 증가시킨다.
		m_fJumpTime += fTimeDelta;
		//처음의 높이 보다 더 내려 갔을때 => 점프전 상태로 복귀한다.
		//if (!m_pColliderCom->Get_IsColl())
		//{
		if (CNavigation::MOVE_OUT != m_pNavigationCom->Check_Navigation(&m_pTransformCom->Get_Position()))
			m_pTransformCom->Move_ToDirection(m_vJumpDir, 5.f, fTimeDelta);
		//}
		m_fAniTime = 0.4f;
		_float fCurY = 0.f;
		m_pTransformCom->Get_CurPositionY(m_pNavigationCom, &fCurY);
		if (m_fPosY + height < -fCurY)
		{
			CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
			if (nullptr == pCollider_Player)
				return;

			if (true == m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
				dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
				dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
				//m_pColliderCom->Set_Coll(false);
			}
			m_fAniTime = 1.f;
			m_fJumpTime = 0.0f;
			m_pTransformCom->Set_PositionY(-fCurY);
			m_vJumpDir = _vec3(0.f, 0.f, 0.f);
			m_bJumpAttackCoolTime = false;
			m_bAttack = false;
		}
		break;
	}
	case SNOWMANATK_ATK:
	{
		if (m_bCheckAttack)
		{
			m_fAttackStartTime += fTimeDelta * 1.f;
			if (m_fAttackStartTime > 1.1f)
			{
				m_bPattern0 = true;
				m_fAttackStartTime = 0.f;
				m_bCheckAttack = false;
				CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
				if (nullptr == pCollider_Player)
					return;
				if (true == m_pMyCollider->Collision_AABB((CCollider*)pCollider_Player))
				{
					auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
					dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
					dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
					m_pMyCollider->Set_Coll(false);
				}
			}
		}
		break;
	}
	}
}

void SnowmanBoss::CollTime(const _float & fTimeDelta)
{
	if (!m_bJumpAttackCoolTime)
	{
		m_fJumpAttackCoolTime += fTimeDelta * 1.f;
		if (m_fJumpAttackCoolTime > 10.f)
		{
			m_bJumpAttackCoolTime = true;
			m_fJumpAttackCoolTime = 0.f;
			m_bPattern0 = false;
		}
	}

	if (!m_bAttackCoolTime2)
	{
		m_fAttackTime2 += fTimeDelta * 1.f;
		if (m_fAttackTime2 > 2.f)
		{
			m_bAttackCoolTime2 = true;
			m_fAttackTime2 = 0.f;
			m_bPattern2 = false;
			m_bCheckAttack = true;
		}
	}
}

HRESULT SnowmanBoss::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

void SnowmanBoss::Collision_ToPlayer()
{
	_vec3 vPos = m_pTargetPlayer->Get_Position();

	if (m_pTransformCom->Compute_Distance(&vPos) < 5.f)
	{
		CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
		if (nullptr == pCollider_Player)
			return;
		if (true == m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player))
		{
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
			dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
			dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
			//m_pColliderCom->Set_Coll(true);
		}
	}
}

SnowmanBoss * SnowmanBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType)
{
	SnowmanBoss *	pInstance = new SnowmanBoss(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(eType)))
	{
		MSG_BOX("SnowmanBoss Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong SnowmanBoss::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTargetPlayer);
	Engine::Safe_Release(m_pMyCollider);

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
