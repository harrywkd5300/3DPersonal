#include "stdafx.h"
#include "..\Headers\Boss.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Explosion.h"
#include "Missile.h"
#include "WindCutter.h"

#include "Management.h"
#include "Scene_Stage.h"

#include "Boss_Paticle.h"

#include "BaseObj.h"
#include "SnowmanBoss.h"
#include "Player.h"

#include "cell.h"

#include "Ending.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_pTargetPlayer(nullptr)
	, m_fTimeDelta(0.f), m_fDelayTime(0.f)
	, m_bMotion(false), m_fBaseAttack(0.f), m_bBaseAttack(true), m_bAttackMotion(true)
	, m_pRightCol(false), m_fHp(300.f), m_fAniTime(1.f), m_pPaticle(nullptr)
{
}

HRESULT CBoss::Ready_GameObject(SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Mesh()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_GameObject()))
		return E_FAIL;

	m_pRHandMatrix = m_pBufferCom->Get_BoneMatrix("RT_MiddleFinger");
	m_pLHandMatrix = m_pBufferCom->Get_BoneMatrix("LT_MiddleFinger");

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	// Stage
	m_pTransformCom->Set_Position(_vec3(48.f, -0.5f, 64.f));

	m_pTransformCom->Scaling(_vec3(0.02f, 0.02f, 0.02f));

	m_pBufferCom->Set_AnimationSet(5);

	m_eNewState = STATE_STAND;

	m_pPaticle = CBoss_Paticle::Create(m_pGraphicDev, m_pTransformCom);
	if (nullptr == m_pPaticle)
		return E_FAIL;

	return NOERROR;
}

_int CBoss::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		CWave_Manager::GetInstance()->Clear_Wave();
		return 1;
	}

	if (!m_bFirst)
		return 0;

	m_fTimeDelta = fTimeDelta;

	if (m_bMakeSnowMan)
	{
		if (!m_bMakeEnd)
		{
			Make_Snowman();
			return 0;
		}
	}

	if (m_bGoBossStage)
	{
		if (!m_bMakeBossEnd)
		{
			Go_Church();
			return 0;
		}
	}

	if (m_fHp > 0)
	{
		Move_Partern(fTimeDelta);

		CollTime(fTimeDelta);

		if (m_bAttackMotion)
			Attack(fTimeDelta);

		if (m_bUsingSkill)
			Using_Skill(fTimeDelta);
	}
	else
	{
		if (m_bMinusHP)
		{
			m_iPass = 2;

			_float vPosY = m_pTransformCom->Get_PositionY();
			m_pTransformCom->Set_PositionY(vPosY - fTimeDelta);
			_float vPosY2 = m_pTransformCom->Get_PositionY();
			if (vPosY2 < 1.3f)
				m_bIsDead = true;
		}
		m_eNewState = STATE_DEATH;
	}

	State_Update();

	CMonster::Update_GameObject(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (SCENE_STAGE == m_eType)
	{
		_vec3 vPos = m_pTransformCom->Get_Position();
	}
	if(!m_bStagePt)
		m_pPaticle->Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);


	return 0;
}

_int CBoss::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (!m_bFirst)
		return 0;

	_matrix matPostion;
	matPostion.Identity();
	m_pTransformCom->Get_WorldMatrix(&matPostion, nullptr);

	m_pRightCol->Set_CircleMatrix((*m_pRHandMatrix * matPostion));
	m_pLeftCol->Set_CircleMatrix((*m_pLHandMatrix * matPostion));
	CMonster::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CBoss::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
	m_pRightCol->Render_CircleBuffer(&_vec3(20.f, 20.f, 20.f));
	m_pLeftCol->Render_CircleBuffer(&_vec3(20.f, 20.f, 20.f));
#endif 

	if(!m_bMinusHP)
		m_pBufferCom->Play_Animation(m_fTimeDelta, m_fAniTime);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	m_iPass = 1;

	m_fAlpha = 1.f;

	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(m_iPass);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	m_pPaticle->Render_GameObject();
}

HRESULT CBoss::Ready_Mesh()
{
	CComponent*			pComponent = nullptr;

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_Boss");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	pComponent = m_pNavigationCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Navigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigtion", pComponent));
	m_pNavigationCom->AddRef();

	return NOERROR;
}

HRESULT CBoss::Ready_Collider()
{
	CComponent*			pComponent = nullptr;

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.RightCollider
	pComponent = m_pRightCol = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_RightCol");
	if (nullptr == pComponent)
		return E_FAIL;
	_matrix matPostion;
	matPostion.Identity();
	m_pTransformCom->Get_WorldMatrix(&matPostion, nullptr);
	m_pRightCol->Set_CircleMatrix((*m_pRHandMatrix * matPostion));
	m_pRightCol->Set_CircleScale(_vec3(20.f, 20.f, 20.f));
	m_pRightCol->Set_ColliderSetting(CCollider::TYPE_CIRCLE, &_vec3(-1.f, -1.f, -1.f), &_vec3(1.f, 1.f, 1.f), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_RightCollider", pComponent));
	m_pRightCol->AddRef();

	// For.RightCollider
	pComponent = m_pLeftCol = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_LeftCol");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pTransformCom->Get_WorldMatrix(&matPostion, nullptr);
	m_pLeftCol->Set_CircleMatrix((*m_pRHandMatrix * matPostion));
	m_pLeftCol->Set_CircleScale(_vec3(20.f, 20.f, 20.f));
	m_pLeftCol->Set_ColliderSetting(CCollider::TYPE_CIRCLE, &_vec3(-1.f, -1.f, -1.f), &_vec3(1.f, 1.f, 1.f), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_LeftCollider", pComponent));
	m_pLeftCol->AddRef();

	return NOERROR;
}

void CBoss::Make_Snowman()
{
	if (m_pStartPos)
	{
		m_pStartPos = false;
		m_pTransformCom->Set_Position(_vec3(48.f, 7.f, 63.8f));
		_float fY = 0.f;
		//CTransform* pObject = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Snowman", 0 , L"Com_Transform");
		
		_vec3 vLook = m_pTransformCom->Get_Look();
		_vec3 vPlayerLook = _vec3(40.f, 0.f, 64.f) - m_pTransformCom->Get_Position();
		
		m_pTransformCom->Get_LookDgreeY(fY, &vLook, &vPlayerLook);

		m_pTransformCom->Rotation_Y(-fY);
	}

	m_fStartY *= m_fTimeDelta;
	if (m_fStartY > 2.f)
		m_bStartY = false;
	if (!m_bStartY)
	{
		_float fY = m_pTransformCom->Get_PositionY();
		m_pTransformCom->Set_PositionY(fY - 1.f * m_fTimeDelta);
		if (fY < 4.f)
		{
			m_bStartY = true;
			m_bMakeMotion = true;
		}
	}
	if (m_bMakeMotion)
	{
		m_fMakeMotion += m_fTimeDelta;
		if (m_fMakeMotion > 1.f)
		{
			m_eNewState = STATE_MAKE;
			m_pPaticle->Update_GameObject(m_fTimeDelta);
			if (m_bAlphaStart)
			{
				m_bAlphaStart = false;
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Object")->GetObjList()->begin();
				auto& iter_end = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_Object")->GetObjList()->end();
				for (; iter != iter_end; ++iter)
				{
					if (!lstrcmp(dynamic_cast<CBaseObj*>((*iter))->Get_ObjData()->FileName, L"SnowMans.X") ||
						!lstrcmp(dynamic_cast<CBaseObj*>((*iter))->Get_ObjData()->FileName, L"SnowMan.X"))
					{
						dynamic_cast<CBaseObj*>((*iter))->Set_AlphaStart();
					}
				}
			}

		}
	}

	State_Update();

	CMonster::Update_GameObject(m_fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

}

void CBoss::Go_Church()
{
	if (m_pStartBossPos)
	{
		m_pStartBossPos = false;
		m_pTransformCom->Set_Position(_vec3(48.f, 4.f, 63.8f));
		_float fY = 0.f;
		CTransform* pObject = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");

		_vec3 vLook = m_pTransformCom->Get_Look();
		_vec3 vPlayerLook = pObject->Get_Position() - m_pTransformCom->Get_Position();

		m_pTransformCom->Get_LookDgreeY(fY, &vLook, &vPlayerLook);

		m_pTransformCom->Rotation_Y(-fY);
	}

	m_fStartZ *= m_fTimeDelta;
	if (m_fStartZ > 2.f)
		m_bStartZ = false;
	if (!m_bStartZ)
	{
		_float fY = m_pTransformCom->Get_PositionX();
		m_pTransformCom->Set_PositionX(fY + 5.f * m_fTimeDelta);
		m_eNewState = STATE_FORWARD;
		if (fY > 68.f)
		{
			m_bStartZ = true;
			m_bFirst = false;
			m_bGoBossStage = false;
			CScene* pScene = CManagement::GetInstance()->Get_CurrentScene();
			dynamic_cast<CScene_Stage*>(pScene)->Set_EndBoss();
			dynamic_cast<CScene_Stage*>(pScene)->Set_EndSnowman();
			CGameObject* pObject = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_SnowBoss")->GetObjList()->front();
			dynamic_cast<SnowmanBoss*>(pObject)->Set_OpenView(false);
			CWave_Manager::GetInstance()->Add_WaveMonster(CWave_Manager::POINT_A, pObject);
			CCamera_Manager::GetInstance()->Change_Camera(L"Static_Camera");
		}
	}

	State_Update();

	CMonster::Update_GameObject(m_fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
}

void CBoss::Move_Partern(const _float& fTimeDelta)
{
	if (nullptr == m_pTargetPlayer)
	{
		m_pTargetPlayer = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pTargetPlayer->AddRef();
	}

	_vec3 vPos = m_pTargetPlayer->Get_Position();

	if (m_bBuildPage)
	{
		m_bStagePt = false;
		m_pTransformCom->Set_Position(_vec3(50.f, 10.f, 80.f));
		//m_pTransformCom->Set_DegreeY();
		m_eNewState = STATE_MAKE;
		dynamic_cast<CBoss_Paticle*>(m_pPaticle)->Set_BoolCrystal2(false);
		CTransform*	pTransform = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_BOSS, L"Layer_Crystal", 0, L"Com_Transform");
		_vec3 vMyLook = _vec3(0.f, 0.f, 0.f);
		_float Dgree = 0.f;
		vMyLook = pTransform->Get_Position() - m_pTransformCom->Get_Position();
		m_pTransformCom->Get_LookDgreeY(Dgree, &vMyLook, &m_pTransformCom->Get_Look());
		if (abs(Dgree) > 2.f)
		{
			m_pTransformCom->Rotation_Y(Dgree * fTimeDelta * 3.5f);
		}
		dynamic_cast<CBoss_Paticle*>(m_pPaticle)->Set_Crystal(&pTransform->Get_Position());
		dynamic_cast<CBoss_Paticle*>(m_pPaticle)->Set_BoolCrystal(true);

		return;
	}
	else
	{
		if (!m_bStagePt)
		{
			_float vPosY = m_pTransformCom->Get_PositionY();
			m_pTransformCom->Set_PositionY(vPosY - fTimeDelta);
			if (m_pTransformCom->Get_PositionY() < 1.7f)
			{
				m_bStagePt = true;
				dynamic_cast<CBoss_Paticle*>(m_pPaticle)->Set_BoolCrystal2(true);
			}
			return;
		}
	}

	if (SCENE_BOSS == m_eType)
		m_pTransformCom->Move_OnNavigation(m_pNavigationCom);

	m_pTransformCom->Set_PositionY(1.7f);

	_float fDist = m_pTransformCom->Compute_Distance(&vPos);

	if (25.f > fDist)
	{
		_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		if (12.f > fDist && m_bSkillCoolTime1)
		{
			if (!m_bPattern1 && m_fHp < 200.f)
			{
				if (!m_bAttack2)
				{
					m_eNewState = STATE_BIGATTACK;
					return;
				}
			}
		}
		if (15.f > fDist && m_bSkillCoolTime2)
		{
			if (!m_bPattern2 && m_fHp < 120.f)
			{
				if (!m_bAttack3)
				{
					m_eNewState = STATE_BIGATTACK2;
					return;
				}
			}
		}
		if(15.f > fDist && m_bSkillCoolTime3)
		{
			if (!m_bPattern3)
			{
				if (!m_bAttack4)
				{
					m_vEndPos = m_pTargetPlayer->Get_Position();
					m_eNewState = STATE_ATTACK2;
					return;
				}
			}
		}

		m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);

		if (abs(fDgree) > 5.f)
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 1.5f);
		else
		{
			if (3.f < fDist)
			{
				if (!m_bPattern0  && m_bSkillEndTime)
				{
					_vec3 vPlayerLook = m_pTargetPlayer->Get_Look();
					D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
					_vec3 vEndPos = vPos + vPlayerLook * 3.f;
					vEndPos.y = 0.f;
					m_pTransformCom->Set_Position(vEndPos);
					m_pNavigationCom->Check_Navigation(&vEndPos);
					m_eNewState = STATE_ATTACK1;
					m_bAttack1 = false;
					m_bPattern0 = true;
					dynamic_cast<CBoss_Paticle*>(m_pPaticle)->Set_DeadPaticle();

				}
				else
				{
					m_eNewState = STATE_FORWARD;
					m_pTransformCom->Move_ToDirection(vLook, 4.f, fTimeDelta);
					_vec3 vGo = m_pTransformCom->Get_Position() + *D3DXVec3Normalize(&vLook, &vLook) *  4.f;
					m_pNavigationCom->Check_Navigation(&vGo);
				}
			}
			else
			{
				if (m_bBaseAttack)
				{
					m_eNewState = STATE_ATTACK;
					m_bBaseAttack = false;
				}
			}
		}
	}
}

void CBoss::Attack(const _float& fTimeDelta)
{
	CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
	if (nullptr == pCollider_Player)
		return;

	switch (m_eCurrState)
	{
	case STATE_ATTACK:
		if (true == m_pRightCol->Collision_CIRCLE((CCollider*)pCollider_Player))
		{
			m_fAttackTime += fTimeDelta * 1.f;
			if (m_fAttackTime > 0.25f)
			{
				m_fAttackTime = 0.f;
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
				dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
				dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMALL_HIT);
			}
		}
		break;
	case STATE_ATTACK1:
		if (true == m_pLeftCol->Collision_CIRCLE((CCollider*)pCollider_Player))
		{
			m_fAttackTime1 += fTimeDelta * 1.f;
			if (m_fAttackTime1 > 0.37f)
			{
				m_fAttackTime1 = 0.f;
				auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
				dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
				dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
			}
		}
		break;
	}

}

void CBoss::CollTime(const _float & fTimeDelta)
{
	if (!m_bBaseAttack)
	{
		m_fBaseAttack += fTimeDelta * 1.f;
		if (m_fBaseAttack > 3.f)
		{
			m_bBaseAttack = true;
			m_fBaseAttack = 0.f;
		}
	}

	if (!m_bAttack1)
	{
		m_fAttack1 += fTimeDelta * 1.f;
		if (m_fAttack1 > 6.f)
		{
			m_bAttack1 = true;
			m_fAttack1 = 0.f;
			m_bPattern0 = false;
		}
	}

	if (m_bAttack2)
	{
		m_fSkillCoolTime1 += fTimeDelta * 1.f;
		if (m_fSkillCoolTime1 > 10.f)
		{
			m_fSkillCoolTime1 = 0.f;
			m_bSkillCoolTime1 = true;
			m_bAttack2 = false;
			m_bPattern1 = false;
		}
	}

	if (m_bAttack3)
	{
		m_fSkillCoolTime2 += fTimeDelta * 1.f;
		if (m_fSkillCoolTime2 > 5.f)
		{
			m_fSkillCoolTime2 = 0.f;
			m_bSkillCoolTime2 = true;
			m_bAttack3 = false;
			m_bPattern2 = false;
		}
	}

	if (m_bAttack4)
	{
		m_fSkillCoolTime3 += fTimeDelta * 1.f;
		if (m_fSkillCoolTime3 > 4.f)
		{
			m_fSkillCoolTime3 = 0.f;
			m_bSkillCoolTime3 = true;
			m_bAttack4 = false;
			m_bPattern3 = false;
		}
	}

	if (!m_bSkillEndTime)
	{
		m_fSkillEndTime4 += fTimeDelta * 1.f;
		if (m_fSkillEndTime4 > 6.f)
		{
			m_bSkillEndTime = true;
			m_fSkillEndTime4 = 0.f;
		}
	}
}

void CBoss::Using_Skill(const _float & fTimeDelta)
{
	switch (e_CurSkill)
	{
	case SKILL_EXPLOSION:
		m_fSkillIdleTime += fTimeDelta * 1.f;
		if (m_fSkillIdleTime > 1.8f)
		{
			m_bUsingSkill = false;
			m_fAniTime = 1.f;
			_vec3 vPos = m_pTransformCom->Get_Position();
			_vec3 vLook = m_pTransformCom->Get_Look();
			D3DXVec3Normalize(&vLook, &vLook);
			_vec3 vStart = vPos + vLook * 8.f;
			vStart.y += 1.5f;

			CGameObject* pObject = CExplosion::Create(m_pGraphicDev, &vStart, m_eType);
			if (nullptr == pObject)
				return;
			CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Layer_Explosion", pObject);
		}
		else
		{
			m_fEndSkill0 += fTimeDelta;
			if (m_fEndSkill0 > 2.f)
			{
				m_bAttack2 = true;
				m_fEndSkill0 = 0.f;
			}
			m_fAniTime = 0.3f;
			return;
		}
		break;
	case SKILL_MISSILE:
		m_fSkillStartTime += fTimeDelta * 1.f;
		if (m_fSkillStartTime > 1.5f)
		{
			m_fSkillMakeTime += fTimeDelta * 1.f;
			if (m_fSkillMakeTime > 0.7f)
			{
				m_fSkillMakeTime = 0.f;

				_vec3 vPos = m_pTransformCom->Get_Position();
				_vec3 vLook = m_pTransformCom->Get_Look();
				D3DXVec3Normalize(&vLook, &vLook);
				vPos += vLook * 1.f;
				vPos.y += 2.f;
				_vec3 vEnd = m_pTargetPlayer->Get_Position();

				vEnd.y += 1.f;

				if (m_fSkillCnt == 1)
					vEnd.z += rand() % 5;
				else if (m_fSkillCnt == 2)
					vEnd.z -= rand() % 5;
				else if (m_fSkillCnt == 3)
					vEnd.x += rand() % 5;
				else
					vEnd.x -= rand() % 5;

				if (4 > m_fSkillCnt)
				{
					CGameObject* pObject = CMissile::Create(m_pGraphicDev, &vPos, &vLook, &vEnd, 0, m_eType);
					if (nullptr == pObject)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Layer_Missile", pObject);
					++m_fSkillCnt;
				}
				else
				{
					m_fSkillStartTime = 0.f;
					m_fSkillCnt = 0.f;
					m_bUsingSkill = false;
				}
			}
		}
		break;
	case SKILL_CUTTER:
		m_fSkillTime3 += fTimeDelta * 1.f;
		if (m_fSkillTime3 > 1.f)
		{
			_vec3 vPos = m_pTransformCom->Get_Position();
			_vec3 vLook = m_pTransformCom->Get_Look();
			D3DXVec3Normalize(&vLook, &vLook);
			vPos += vLook * 1.f;
			vPos.y += 0.5f;

			CGameObject* pObject = WindCutter::Create(m_pGraphicDev, &vPos, &vLook, &m_vEndPos, m_eType);
			if (nullptr == pObject)
				return;
			CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Layer_WindCutter", pObject);

			m_fSkillTime3 = 0.f;

			m_bUsingSkill = false;
		}
	}
}

void CBoss::Set_Position(_vec3 * vPos)
{
	m_pTransformCom->Set_Position(*vPos);
}

void CBoss::Set_Scale(_vec3 * vPos)
{
	m_pTransformCom->Scaling(*vPos);
}

void CBoss::Release_Component()
{
	Engine::Safe_Release(m_pMaterialCom);
	Delete_Component(L"Com_Material");
	Engine::Safe_Release(m_pTransformCom);
	Delete_Component(L"Com_Transform");
	Engine::Safe_Release(m_pBufferCom);
	Delete_Component(L"Com_Buffer");
	Engine::Safe_Release(m_pShaderCom);
	Delete_Component(L"Com_Shader");
	Engine::Safe_Release(m_pColliderCom);
	Delete_Component(L"Com_Collider");
	Engine::Safe_Release(m_pRendererCom);
	Delete_Component(L"Com_Renderer");
	Engine::Safe_Release(m_pNavigationCom);
	Delete_Component(L"Com_Navigtion");
	Engine::Safe_Release(m_pRightCol);
	Delete_Component(L"Com_RightCollider");
	Engine::Safe_Release(m_pLeftCol);
	Delete_Component(L"Com_LeftCollider");

	m_eType = SCENE_BOSS;

	Ready_Component();
	Ready_Mesh();
	Ready_Collider();

	m_pRHandMatrix = m_pBufferCom->Get_BoneMatrix("RT_MiddleFinger");
	m_pLHandMatrix = m_pBufferCom->Get_BoneMatrix("LT_MiddleFinger");

	_vec3 vPosA = *m_pNavigationCom->Get_Cell(30)->Get_Point(CCell::POINT_A);
	_vec3 vPosB = *m_pNavigationCom->Get_Cell(30)->Get_Point(CCell::POINT_B);
	_vec3 vPosC = *m_pNavigationCom->Get_Cell(30)->Get_Point(CCell::POINT_C);

	_vec3 vPos = _vec3((vPosA.x + vPosB.x + vPosC.x) / 3.f,
		(vPosA.y + vPosB.y + vPosC.y) / 3.f,
		(vPosA.z + vPosB.z + vPosC.z) / 3.f);

	vPos.y += 1.f;

	m_pTransformCom->Set_Position(vPos);
}

void CBoss::State_Update()
{
	if (STATE_STAND != m_eCurrState && STATE_FORWARD != m_eCurrState &&
	    STATE_TURN != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = STATE_STAND;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}
			if (STATE_BIGATTACK == m_eCurrState)
			{
				if (true == m_bPattern1)
					m_bPattern1 = false;

				m_bSkillCoolTime1 = false;
				m_bPattern1 = true;
			}

			if (STATE_BIGATTACK2 == m_eCurrState)
			{
				if (true == m_bPattern2)
					m_bPattern2 = false;

				m_bSkillCoolTime2 = false;
				m_bAttack3 = true;
			}
			if (STATE_ATTACK2 == m_eCurrState)
			{
				m_bSkillEndTime = false;
				m_bSkillCoolTime3 = true;
				m_bPattern3 = true;
				m_bAttack4 = true;
			}
			if (STATE_DEATH == m_eCurrState)
			{
				m_bMinusHP = true;
			}
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case STATE_STAND:
			m_pBufferCom->Set_AnimationSet(5);
			break;
		case STATE_FORWARD:
			m_pBufferCom->Set_AnimationSet(0);
			break;
		case STATE_ATTACK:
			m_pBufferCom->Set_AnimationSet(4);
			m_fDelayTime = 0.4f;
			m_bAttackMotion = true;
			m_bMotion = true;
			break;
		case STATE_TURN:
			m_pBufferCom->Set_AnimationSet(1);
			break;
		case STATE_DEATH:
			m_pBufferCom->Set_AnimationSet(9);
			m_fDelayTime = 0.3f;
			m_bMotion = true;
			break;
		case STATE_HURT:
			m_pBufferCom->Set_AnimationSet(6);
			m_bMotion = true;
			break;
		case STATE_ATTACK1:
			m_pBufferCom->Set_AnimationSet(3);
			m_fDelayTime = 0.22f;
			m_bAttackMotion = true;
			m_bMotion = true;
			break;
		case STATE_ATTACK2:
			m_pBufferCom->Set_AnimationSet(2);
			m_fDelayTime = 0.3f;
			m_bMotion = true;
			m_bUsingSkill = true;
			e_CurSkill = SKILL_CUTTER;
			break;
		case STATE_BIGATTACK:
			m_pBufferCom->Set_AnimationSet(7);
			m_fDelayTime = 0.1f;
			m_bMotion = true;
			m_bUsingSkill = true;
			e_CurSkill = SKILL_EXPLOSION;
			break;
		case STATE_BIGATTACK2:
			m_pBufferCom->Set_AnimationSet(8);
			m_fDelayTime = 0.25f;
			m_bMotion = true;
			m_bUsingSkill = true;
			e_CurSkill = SKILL_MISSILE;
			break;
		case STATE_MAKE:
			m_pBufferCom->Set_AnimationSet(11);
			//m_fDelayTime = 0.3f;
			m_bMotion = true;
		}

		m_eCurrState = m_eNewState;
	}


}

HRESULT CBoss::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

	pEffect->SetFloat("g_fSize", 10.f);

	Safe_Release(pEffect);

	return NOERROR;
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType)
{
	CBoss *	pInstance = new CBoss(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(eType)))
	{
		MSG_BOX("CBoss Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CBoss::Free()
{
	_ulong		dwRefCnt = 0;
	
	Engine::Safe_Release(m_pTargetPlayer);
	Engine::Safe_Release(m_pRightCol);
	Engine::Safe_Release(m_pLeftCol);
	Engine::Safe_Release(m_pPaticle);

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
