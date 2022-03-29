#include "stdafx.h"
#include "..\Headers\LightningTower.h"

#include "Component_Manager.h"
#include "object_Manager.h"

#include "Layer.h"
#include "Boss.h"

#include "LightningBall.h"
#include "Wave_Manager.h"

#include "Sound_Manager.h"

CLightningTower::CLightningTower(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pPlayerTrans(nullptr), m_pWave_Manager(CWave_Manager::GetInstance())
{

}

HRESULT CLightningTower::Ready_GameObject(_vec3* vPos, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Position(*vPos);
	m_pTransformCom->Scaling(_vec3(0.005f, 0.005f, 0.005f));

	m_pBufferCom->Set_AnimationSet(0);

	return NOERROR;
}

_int CLightningTower::Update_GameObject(const _float & fTimeDelta)
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

		Move_Partern(fTimeDelta);
		if (m_bAttack)
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

_int CLightningTower::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;



	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CLightningTower::Render_GameObject(void)
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

HRESULT CLightningTower::Ready_Component()
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
	pComponent = m_pBufferCom = (Engine::CDynamic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_LightningTower");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(270.f, 180.f, 0.f));
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

HRESULT CLightningTower::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

void CLightningTower::Not_BuildMove()
{
	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	_vec3 vPos = m_pPlayerTrans->Get_Position();
	_vec3 vLook = m_pPlayerTrans->Get_Look();

	vPos += *D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom->Set_Position(vPos);
}

void CLightningTower::Collision_ToPlayer()
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

void CLightningTower::State_Update()
{
	if (STRIKE_STAND != m_eCurrState)
	{
		if (!m_pBufferCom->End_Animation(m_fDelayTime))
			return;
		else
		{
			m_eNewState = STRIKE_STAND;

			if (true == m_bMotion)
			{
				m_bMotion = false;
				m_fDelayTime = 0.f;
			}
		}
	}
	if (m_eNewState != m_eCurrState)
	{
		switch (m_eNewState)
		{
		case STRIKE_STAND:
			m_pBufferCom->Set_AnimationSet(0);
			break;
		case STRIKE_ATTAKC:
			m_pBufferCom->Set_AnimationSet(1);
			break;
		}
		m_eCurrState = m_eNewState;
	}
}

void CLightningTower::Move_Partern(const _float & fTimeDelta)
{
	if (m_pWave_Manager->Get_WaveList()->empty())
		return;

	_uint iiDX = m_pWave_Manager->Get_WaveList()->size();

	auto& iter = m_pWave_Manager->Get_WaveList()->begin();
	auto& iter_end = m_pWave_Manager->Get_WaveList()->end();

	for (; iter != iter_end; ++iter)
	{
		CTransform* pMonsterTrans = (CTransform*)(*iter)->Get_Component(L"Com_Transform");

		if (m_pTransformCom->Compute_Distance(&pMonsterTrans->Get_Position()) < 15.f)
		{
			m_vTargetPos = pMonsterTrans->Get_Position();
			_vec3 vLook = m_vTargetPos - m_pTransformCom->Get_Position(); // 플레이어보는 룩
			_vec3 vMyLook = m_pTransformCom->Get_Look();
			_float fDgree = 0.f;
			m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);
			if (abs(fDgree) > 5.f)
				m_pTransformCom->Rotation_Y(fDgree * fTimeDelta * 2.f);
			else
			{
				m_eNewState = STRIKE_ATTAKC;
				if (m_bMakeAttack)
				{
					m_bMakeAttack = false;
					_vec3 vPos = m_pTransformCom->Get_Position();
					m_pBall = CLighningBall::Create(m_pGraphicDev, &vPos, this, m_eType);
					if (nullptr == m_pBall)
						return;
					CObject_Manager::GetInstance()->Ready_GameObject(m_eType, L"Layer_LightingBall", m_pBall);
				}
			}
			break;
		}
		
		if (iter == iter_end)
		{
			m_eNewState = STRIKE_STAND;
			m_vTargetPos = _vec3(0.f, 0.f, 0.f);
		}
	}
	//if (nullptr != m_pTargetMonster)
	//{
	//	_vec3 vLook = m_pTargetMonster->Get_Position() - m_pTransformCom->Get_Position(); // 플레이어보는 룩
	//	_vec3 vMyLook = m_pTransformCom->Get_Look();
	//	_float fDgree = 0.f;
	//	m_pTransformCom->Get_LookDgreeY(fDgree, &vLook, &vMyLook);
	//	if (abs(fDgree) > 5.f)
	//		m_pTransformCom->Rotation_Y(fDgree * fTimeDelta);
	//	else
	//	{
	//		if (m_bMakeAttack)
	//		{
	//			m_bMakeAttack = false;
	//			_vec3 vPos = m_pTransformCom->Get_Position();
	//			m_pBall = CLighningBall::Create(m_pGraphicDev, &vPos, this, m_eType);
	//			if (nullptr == m_pBall)
	//				return;
	//			CObject_Manager::GetInstance()->Ready_GameObject(SCENE_STAGE, L"Layer_LightingBall", m_pBall);
	//		}
	//	}
	//}
}

void CLightningTower::Attack(const _float & fTimeDelta)
{
	if (m_bAttack)
	{
		m_eNewState = STRIKE_STAND;
		if (m_bSettingMonster)
		{
			if (m_vTargetPos.x != 0)
			{
				m_bSettingMonster = false;
				dynamic_cast<CLighningBall*>(m_pBall)->Set_EndPos(&m_vTargetPos);
				CSound_Manager::GetInstance()->Play_Sound(L"Crystal_Activate.ogg", CSound_Manager::Channel_TowerAtk0, 1.f);
			}
		}
	}
}

void CLightningTower::CollTime(const _float & fTimeDelta)
{
	if (m_bAttackCoolTime)
	{
		m_fAttackCoolTime += fTimeDelta * 1.f;
		if (m_fAttackCoolTime > 1.5f)
		{
			m_bMakeAttack = true;
			m_bAttackCoolTime = false;
			m_fAttackCoolTime = 0.f;
			m_bSettingMonster = true;
			m_bAttack = false;
		}
	}
}

CLightningTower * CLightningTower::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eType)
{
	CLightningTower*	pInstance = new CLightningTower(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPos, eType)))
	{
		MSG_BOX("CLightningTower Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CLightningTower::Free()
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
