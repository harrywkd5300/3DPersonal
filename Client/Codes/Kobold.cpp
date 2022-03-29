#include "stdafx.h"
#include "..\Headers\Kobold.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Player.h"
#include "cell.h"
#include "Shield.h"

#include "Fire_Explosion.h"

//OrcBruiser

CKobold::CKobold(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev), m_pTargetPlayer(nullptr)
	, m_fTimeDelta(0.f), m_fDelayTime(0.f), m_fAniTime(1.f)
	, m_pWave_Manager(CWave_Manager::GetInstance())
{
}

HRESULT CKobold::Ready_GameObject(const _uint& iIdx)
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

	m_pTransformCom->Scaling(_vec3(0.3f, 0.3f, 0.3f));

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	m_eNewState = KOBOLD_IDLE;

	m_fHp = 300.f;

	return NOERROR;
}

_int CKobold::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		_vec3 vPos = m_pTransformCom->Get_Position();
		_vec3 vScale = _vec3(3.5f, 3.5f, 3.5f);

		CWave_Manager::GetInstance()->MonsterDead(this);

		CGameObject* pObject = CFire_Explosion::Create(m_pGraphicDev, &vPos, &vScale, 14, m_eType);
		if (nullptr == pObject)
			return 1;
		CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Test_Explosion", pObject);
		return 1;
	}
	//if (!m_bFirst)
	//	return 0;

	m_fTimeDelta = fTimeDelta;

	CMonster::Update_GameObject(fTimeDelta);

	if (m_fHp > 0)
	{
		//Collision_Monster();

		Move_Partern(fTimeDelta);

		if (m_bAttack)
			Attack(fTimeDelta);
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
		m_fAlpha -= fTimeDelta * 1.f;
		m_eNewState = KOBOLD_DEATH;
	}

	State_Update();

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CKobold::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CMonster::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CKobold::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

	m_pMaterialCom->SetUp_OnGraphicDev();

#ifdef _DEBUG
	m_pColliderCom->Render_Buffer();
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

HRESULT CKobold::Ready_Mesh()
{
	CComponent*			pComponent = nullptr;

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Mesh_Kobold");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

HRESULT CKobold::Ready_Collider()
{
	CComponent*			pComponent = nullptr;

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(90.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	return NOERROR;
}

HRESULT CKobold::Ready_Navigation(const _uint & iIdx)
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

void CKobold::State_Update()
{
	if (KOBOLD_IDLE != m_eCurrState && KOBOLD_TURN != m_eCurrState && KOBOLD_FORWARD != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = KOBOLD_IDLE;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}

			if (KOBOLD_ATTACKSTART == m_eCurrState)
				m_eNewState = KOBOLD_ATTACK0;


			if (KOBOLDSD_ATTACKSTART == m_eCurrState)
				m_eNewState = KOBOLDSD_ATTACK0;
		
			if (KOBOLD_DEATH == m_eCurrState)
				m_bMinusHP = true;
		}
	}

	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case KOBOLD_IDLE:
			m_pBufferCom->Set_AnimationSet(0);
			//m_fDelayTime = 0.6f;
			break;
		case KOBOLD_FORWARD:
			m_pBufferCom->Set_AnimationSet(1);
			break;
		case KOBOLD_DEATH:
			m_pBufferCom->Set_AnimationSet(6);
			m_fDelayTime = 0.2f;
			m_bMotion = true;
			break;
		case KOBOLD_HURT:
			m_pBufferCom->Set_AnimationSet(5);
			m_bMotion = true;
			break;
		case KOBOLD_SHOCK:
			m_pBufferCom->Set_AnimationSet(9);
			m_bMotion = true;
			break;
		case KOBOLD_ATTACKSTART:
			m_pBufferCom->Set_AnimationSet(10);
			m_bMotion = true;
			break;
		case KOBOLD_ATTACK0:
			m_pBufferCom->Set_AnimationSet(8);
			m_bMotion = true;
			m_bAttack = true;
			m_eAtkState = KOBOLDATK_ATK0;
			break;
		case KOBOLDSD_ATTACKSTART:
			m_pBufferCom->Set_AnimationSet(10);
			m_bMotion = true;
			break;
		case KOBOLDSD_ATTACK0:
			m_pBufferCom->Set_AnimationSet(8);
			m_bMotion = true;
			m_bAttack = true;
			m_eAtkState = KOBOLDATKSD_ATK0;
			break;
		case KOBOLD_TURN:
			m_pBufferCom->Set_AnimationSet(4);
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void CKobold::Move_Partern(const _float & fTimeDelta)
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
		for (size_t i = 0; i < iSize; ++i)
		{
			CTransform*		pTrans_Shild = (CTransform*)CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Shield", i, L"Com_Transform");
			if (nullptr == pTrans_Shild)
				return;
			_vec3 vShiedPos = pTrans_Shild->Get_Position();
			_float fDist = m_pTransformCom->Compute_Distance(&vShiedPos);
			if (fDist < 5.f)
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

	if (4.f > fDist)
	{
		_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);

		if (abs(fDgree) > 5.f)
		{
			m_eNewState = KOBOLD_TURN;
			m_pTransformCom->Rotation_Y(fDgree * fTimeDelta);
		}
		else
		{
			if (7.0f < fDist)
			{
				m_eNewState = KOBOLD_FORWARD;
				m_pTransformCom->Move_ToDirection(vLook, 4.f, fTimeDelta);
			}
			else
			{
				if (7.0f > fDist)
				{
					m_eNewState = KOBOLD_ATTACKSTART;
				}
				else
					m_eNewState = KOBOLD_IDLE;
			}
		}
	}
	else if (fShieldDist != 0.f)
	{
		m_eNewState = KOBOLDSD_ATTACKSTART;
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
			if (m_pNavigationCom->Get_VecCell()->size() - 1 > m_iNaviIdx)
				++m_iNaviIdx;
		}

		_vec3 vMyLook = m_pTransformCom->Get_Look();

		_float fDgree = 0.f;

		m_pTransformCom->Get_LookDgreeY(fDgree, &vDir, &vMyLook);

		if (abs(fDgree) > 5.f)
		{
			m_eNewState = KOBOLD_TURN;
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
			m_eNewState = KOBOLD_FORWARD;
			_uint iRd = rand() % 5;
			if (!m_pColliderCom->Get_IsColl() && !m_bColMove)
			{
				_vec3 vPos = *m_pColliderCom->Get_Radius();
				_vec3 vColPos = m_pTransformCom->Get_Position() + vPos;
				_vec3 vLook = vColPos - m_pTransformCom->Get_Position();
				if (D3DXVec3Length(&vLook) < 2.f)
					m_pTransformCom->Move_ToDirection(vDir, _float(iRd), fTimeDelta);
				else
					m_pTransformCom->Move_ToDirection(-vLook, _float(iRd), fTimeDelta);
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

void CKobold::Attack(const _float & fTimeDelta)
{
	switch (m_eAtkState)
	{
	case KOBOLDATK_ATK0:
	{
		m_bAttackCoolTime0 = false;
		m_bAttackMove = true;
		_vec3 vPos = m_pTargetPlayer->Get_Position();
		_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
		m_pTransformCom->Move_ToDirection(vLook, 6.f, fTimeDelta);
		_float fDist = m_pTransformCom->Compute_Distance(&vPos);
		if (fDist < 1.f)
			m_bIsDead = true;
	}
	break;
	case KOBOLDATKSD_ATK0:
	{
		if (m_pShield != nullptr)
		{
			m_bAttackCoolTime0 = false;
			m_bAttackMove = true;

			CTransform* pComponent = (CTransform*)dynamic_cast<CShield*>(m_pShield)->Get_Component(L"Com_Transform");
			_vec3 vPos = pComponent->Get_Position();
			_vec3 vLook = vPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
			m_pTransformCom->Move_ToDirection(vLook, 6.f, fTimeDelta);
			_float fDist = m_pTransformCom->Compute_Distance(&vPos);
			if (fDist < 1.f)
				m_bIsDead = true;
		}
		else
			m_eNewState = KOBOLD_IDLE;
	}
	break;
	}
}

void CKobold::Collision_Monster()
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

HRESULT CKobold::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

CKobold * CKobold::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx)
{
	CKobold *	pInstance = new CKobold(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(iIdx)))
	{
		MSG_BOX("CKobold Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CKobold::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTargetPlayer);

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
