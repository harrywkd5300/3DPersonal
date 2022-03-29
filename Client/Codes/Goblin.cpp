#include "stdafx.h"
#include "..\Headers\Goblin.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Player.h"
#include "cell.h"
#include "Shield.h"

#include "Wave_Manager.h"

CGoblin::CGoblin(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev), m_pTargetPlayer(nullptr)
	, m_fTimeDelta(0.f), m_fDelayTime(0.f), m_fAniTime(1.f)
	, m_pWave_Manager(CWave_Manager::GetInstance())
{
}

HRESULT CGoblin::Ready_GameObject(const _uint& iIdx)
{
	if (FAILED(Ready_Mesh()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Navigation(iIdx)))
		return E_FAIL;

	_vec3 vPosA = *m_pNavigationCom->Get_Cell(0)->Get_Point(CCell::POINT_A);
	_vec3 vPosB = *m_pNavigationCom->Get_Cell(0)->Get_Point(CCell::POINT_B);
	_vec3 vPosC = *m_pNavigationCom->Get_Cell(0)->Get_Point(CCell::POINT_C);

	_vec3 vPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
		(vPosA.y + vPosB.y + vPosC.y) / 3.f,
		(vPosA.z + vPosB.z + vPosC.z) / 3.f);

	_uint iX = rand() % 3;
	_uint iZ = rand() % 3;

	vPos.x += _float(iX);
	vPos.z += _float(iZ);

	// Stage
	m_pTransformCom->Set_Position(vPos);

	m_pTransformCom->Scaling(_vec3(0.045f, 0.045f, 0.045f));

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	m_eNewState = GOBLIIN_IDLE;

	m_fHp = 300.f;

	return NOERROR;
}

_int CGoblin::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		CWave_Manager::GetInstance()->MonsterDead(this);
		return 1;
	}

	m_fTimeDelta = fTimeDelta;
	
	CMonster::Update_GameObject(fTimeDelta);

	if (m_fHp > 0)
	{
		//Collision_Monster();

		Move_Partern(fTimeDelta);
		if (m_bAttack)
			Attack(fTimeDelta);

		CollTime(fTimeDelta);
	}
	else
	{
		m_iPass = 2;
		if (m_bMinusHP)
		{
			if (m_fAlpha < 0.1f)
			{
				m_bIsDead = true;
			}
		}
		m_eNewState = GOBLIIN_DEATH;
		m_fAlpha -= fTimeDelta * 1.f;
	}

	State_Update();

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CGoblin::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CMonster::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CGoblin::Render_GameObject(void)
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

	if (!m_bMinusHP)
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

HRESULT CGoblin::Ready_Mesh()
{
	CComponent*			pComponent = nullptr;

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Mesh_Goblin");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

HRESULT CGoblin::Ready_Collider()
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
	vMin.z -= 14.f;
	_vec3 vMax = *m_pBufferCom->Get_Max();
	vMax *= 1.5f;
	vMax.z += 14.f;
	m_pMyCollider->Set_ColliderSetting(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MyCollider", pComponent));
	m_pMyCollider->AddRef();

	return NOERROR;
}

HRESULT CGoblin::Ready_Navigation(const _uint & iIdx)
{
	CComponent*			pComponent = nullptr;

	if (0 == iIdx)
	{
		// For.Navigation_POINTA
		pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation_PointA");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
		m_pNavigationCom->AddRef();
	}
	else if (1 == iIdx)
	{
		// For.Navigation_POINTA
		pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation_PointB");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
		m_pNavigationCom->AddRef();
	}
	else
	{
		// For.Navigation_POINTA
		pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Navigation_PointC");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
		m_pNavigationCom->AddRef();
	}

	return NOERROR;
}

void CGoblin::State_Update()
{
	if (GOBLIIN_IDLE != m_eCurrState && GOBLIIN_LEFT != m_eCurrState && GOBLIIN_FORWARD != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = GOBLIIN_IDLE;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}
			if (GOBLIIN_ATTACK1 == m_eCurrState)
			{
				m_bAttackMove = false;
			}

			if (GOBLIIN_ATTACK2 == m_eCurrState)
			{
				m_bAttackMove = false;
			}


			if (GOBLIIN_DEATH == m_eCurrState)
			{
				m_bMinusHP = true;
			}
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case GOBLIIN_IDLE:
			m_pBufferCom->Set_AnimationSet(4);
			//m_fDelayTime = 0.6f;
			break;
		case GOBLIIN_FORWARD:
			m_pBufferCom->Set_AnimationSet(2);
			break;
		case GOBLIIN_DEATH:
			m_pBufferCom->Set_AnimationSet(6);
			m_fDelayTime = 0.3f;
			m_bMotion = true;
			break;
		case GOBLIIN_HURT:
			m_pBufferCom->Set_AnimationSet(5);
			m_bMotion = true;
			break;
		case GOBLIIN_SHOCK:
			m_pBufferCom->Set_AnimationSet(1);
			m_bMotion = true;
			break;
		case GOBLIIN_ATTACK1:
			m_pBufferCom->Set_AnimationSet(10);
			m_bMotion = true;
			m_bAttack = true;
			m_eAtkState = GOBLIINATK_ATK0;
			break;
		case GOBLIIN_ATTACK2:
			m_pBufferCom->Set_AnimationSet(11);
			m_bMotion = true;
			m_bAttack = true;
			m_eAtkState = GOBLIINATK_ATK1;
			break;
		case GOBLIIN_ATTACK3:
			m_pBufferCom->Set_AnimationSet(11);
			m_bMotion = true;
			m_bAttack = true;
			m_eAtkState = GOBLIINATK_SHIELD;
			break;
		case GOBLIIN_LEFT:
			m_pBufferCom->Set_AnimationSet(0);
			break;
		case GOBLIIN_CHEER:
			m_pBufferCom->Set_AnimationSet(9);
			m_bMotion = true;
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void CGoblin::Move_Partern(const _float & fTimeDelta)
{
	if (nullptr == m_pTargetPlayer)
	{
		m_pTargetPlayer = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pTargetPlayer->AddRef();
	}

	// Shield 거리 재는 위치
	_float	fShieldDist = 0.f;

	if (nullptr != CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Shield"))
	{
		_uint iSize = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Shield")->GetObjList()->size();
		for (size_t i = 0; i < iSize ; ++i)
		{
			CTransform*		pTrans_Shild = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Shield", i, L"Com_Transform");
			if (nullptr == pTrans_Shild)
				return;
			_vec3 vShiedPos = pTrans_Shild->Get_Position();
			_float fDist = m_pTransformCom->Compute_Distance(&vShiedPos);
			if (fDist < 2.f)
			{
				auto iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Shield")->GetObjList()->begin();
				for (size_t j = 0; j < i; ++j)
					iter++;
				m_pShield = (*iter);
				fShieldDist = fDist;
				break;
			}
			m_pShield = nullptr;
		}
	}


	_vec3 vPos = m_pTargetPlayer->Get_Position();

	_float fDist = m_pTransformCom->Compute_Distance(&vPos);

	if (5.f > fDist)
	{
		_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);

		if (abs(fDgree) > 5.f)
		{
			m_eNewState = GOBLIIN_LEFT;
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
			m_pTransformCom->Move_ToDirection(vLook, 0.5f, fTimeDelta);
		}
		else
		{
			if (1.0f < fDist && !m_bAttackMove)
			{
				m_eNewState = GOBLIIN_FORWARD;
				m_pTransformCom->Move_ToDirection(vLook, 4.f, fTimeDelta);
			}
			else
			{
				if (m_bAttackCoolTime1)
				{
					m_eNewState = GOBLIIN_ATTACK2;
				}
				else if (m_bAttackCoolTime0)
				{
					m_eNewState = GOBLIIN_ATTACK1;
				}
				else
					m_eNewState = GOBLIIN_IDLE;
			}
		}
	}
	else if (fShieldDist != 0.f)
	{
		if (m_bAttackCoolTime1)
		{
			m_eNewState = GOBLIIN_ATTACK3;
		}
		else

			m_eNewState = GOBLIIN_IDLE;
	}
	else
	{
		_vec3 vPosA = *m_pNavigationCom->Get_Cell(m_iNaviIdx)->Get_Point(CCell::POINT_A);
		_vec3 vPosB = *m_pNavigationCom->Get_Cell(m_iNaviIdx)->Get_Point(CCell::POINT_B);
		_vec3 vPosC = *m_pNavigationCom->Get_Cell(m_iNaviIdx)->Get_Point(CCell::POINT_C);

		_vec3 vPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
			(vPosA.y + vPosB.y + vPosC.y) / 3.f,
			(vPosA.z + vPosB.z + vPosC.z) / 3.f);


		_vec3 vDir = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		

		if (D3DXVec3Length(&vDir) < 1.f)
		{
			if(m_pNavigationCom->Get_VecCell()->size() - 1 > m_iNaviIdx)
				++m_iNaviIdx;
		}

		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		m_pTransformCom->Get_LookDgreeY(fDgree, &vDir, &vMyLook);

		if (abs(fDgree) > 5.f)
		{
			m_eNewState = GOBLIIN_LEFT;
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
			if (!m_pColliderCom->Get_IsColl() && !m_bColMove)
				m_pTransformCom->Move_ToDirection(vDir, 0.5f, fTimeDelta);
			else
			{
				_vec3 vPos = *m_pColliderCom->Get_Radius();
				_vec3 vColPos = m_pTransformCom->Get_Position() + vPos;
				_vec3 vLook = vColPos - m_pTransformCom->Get_Position();
				m_pTransformCom->Move_ToDirection(vLook, 0.5f, fTimeDelta);  // 충돌시
			}
		}
		else
		{
			m_eNewState = GOBLIIN_FORWARD;
			_uint iRd = rand() % 5;
			if (!m_pColliderCom->Get_IsColl() && !m_bColMove)
			{
				_vec3 vPos = *m_pColliderCom->Get_Radius();
				_vec3 vColPos = m_pTransformCom->Get_Position() + vPos;
				_vec3 vLook = vColPos - m_pTransformCom->Get_Position();
				_float fSize = D3DXVec3Length(&vLook);
				if (fSize < 2.f)
					m_pTransformCom->Move_ToDirection(vDir, _float(iRd), fTimeDelta);
				else
				{
					m_pTransformCom->Move_ToDirection(-vLook, _float(iRd), fTimeDelta);
					//cout << "Goblin : " << fSize << endl;
				}
			}
			//else
			//{
			//	_vec3 vPos = *m_pColliderCom->Get_Radius();
			//	_vec3 vColPos = m_pTransformCom->Get_Position() + vPos;
			//	_vec3 vLook = vColPos - m_pTransformCom->Get_Position();
			//	m_pTransformCom->Move_ToDirection(-vLook, _float(iRd), fTimeDelta);  // 충돌시
			//}
			if (m_bColMove)
			{
				m_fColTime += fTimeDelta * 1.f;
				if (m_fColTime > 1.5f)
				{
					m_fColTime = 0.f;
					m_bColMove = false;
				}
			}
		}
	}
}

void CGoblin::Attack(const _float & fTimeDelta)
{
	switch (m_eAtkState)
	{
	case GOBLIINATK_ATK0:
	{
		m_bAttackCoolTime0 = false;
		m_bAttackMove = true;
		m_fAttackStartTime0 += fTimeDelta * 1.f;
		if (m_fAttackStartTime0 > 0.6f)
		{
			m_fAttackStartTime0 = 0.f;
			CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
			if (nullptr == pCollider_Player)
				return;
			if (true == m_pMyCollider->Collision_AABB((CCollider*)pCollider_Player))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
				dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
				dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMALL_HIT);
				m_pMyCollider->Set_Coll(false);
				m_bAttack = false;
			}
		}
	}
		break;
	case GOBLIINATK_ATK1:
	{
		m_bAttackCoolTime1 = false;
		m_bAttackMove = true;
		m_fAttackStartTime1 += fTimeDelta * 1.f;
		if (m_fAttackStartTime1 > 0.6f)
		{
			m_fAttackStartTime1 = 0.f;
			CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
			if (nullptr == pCollider_Player)
				return;
			if (true == m_pMyCollider->Collision_AABB((CCollider*)pCollider_Player))
			{
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
				dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
				dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
				m_pMyCollider->Set_Coll(false);
				m_bAttack = false;
			}
		}
	}
		break;
	case GOBLIINATK_SHIELD:
		m_bAttackCoolTime1 = false;
		m_bAttackMove = true;
		m_fAttackStartTime1 += fTimeDelta * 1.f;
		if (m_fAttackStartTime1 > 0.6f)
		{
			m_fAttackStartTime1 = 0.f;
			if (nullptr != m_pShield)
			{
				CComponent*		pCollider_Shiled = dynamic_cast<CShield*>(m_pShield)->Get_Collider();
				if (nullptr == pCollider_Shiled)
					return;
				if (true == m_pMyCollider->Collision_AABB((CCollider*)pCollider_Shiled))
				{
					dynamic_cast<CShield*>(m_pShield)->Set_IsDead();
					m_pShield = nullptr;
				}
			}
		}
		break;
	}
}

void CGoblin::CollTime(const _float & fTimeDelta)
{
	if (!m_bAttackCoolTime0)
	{
		m_fAttackCoolTime0 += fTimeDelta * 1.f;
		if (m_fAttackCoolTime0 > 2.f)
		{
			m_fAttackCoolTime0 = 0.f;
			m_bAttackCoolTime0 = true;
		}
	}

	if (!m_bAttackCoolTime1)
	{
		m_fAttackCoolTime1 += fTimeDelta * 1.f;
		if (m_fAttackCoolTime1 > 5.f)
		{
			m_fAttackCoolTime1 = 0.f;
			m_bAttackCoolTime1 = true;
		}
	}

}

void CGoblin::Collision_Monster()
{
	auto& iter = m_pWave_Manager->Get_WaveList()->begin();
	auto& iter_end = m_pWave_Manager->Get_WaveList()->end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter) == this)
			continue;

		CTransform*		pCollider_Monster = (CTransform*)(*iter)->Get_Component(L"Com_Transform");
		if (nullptr == pCollider_Monster)
			return;
		if (m_pTransformCom->Compute_Distance(&pCollider_Monster->Get_Position()) < 5.f)
		{
			CComponent*		pCollider = (*iter)->Get_Component(L"Com_Collider");
			if (true == m_pColliderCom->Collision_AABB((CCollider*)pCollider))
			{
				m_pColliderCom->Set_Radius((CCollider*)pCollider);
				m_pColliderCom->Set_Coll(true);
				return;
			}
			m_fCollState = 1.f;
		}
	}
	m_pColliderCom->Set_Coll(false);
}

HRESULT CGoblin::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

CGoblin * CGoblin::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx)
{
	CGoblin *	pInstance = new CGoblin(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(iIdx)))
	{
		MSG_BOX("CGoblin Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CGoblin::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTargetPlayer);
	Engine::Safe_Release(m_pMyCollider);


	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}

