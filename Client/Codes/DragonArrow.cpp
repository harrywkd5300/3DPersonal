#include "stdafx.h"
#include "..\Headers\DragonArrow.h"

#include "Component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Monster.h"
#include "DragonKiller.h"

#include "Wave_Manager.h"

CDragonArrow::CDragonArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr), m_pBufferCom(nullptr), m_pColliderCom(nullptr)
	, m_pRendererCom(nullptr), m_pShaderCom(nullptr)
	, m_vDir(0.f, 0.f, 0.f), m_fDist(0.f), m_pWave_Manager(CWave_Manager::GetInstance())
{
}

HRESULT CDragonArrow::Ready_GameObject(_vec3* vStartPos, _vec3* vDir, _vec3* vRot, CGameObject* pObject, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pDragonKiller = pObject;

	m_vDir = *vDir;

	_vec3 vLook;
	D3DXVec3Normalize(&vLook, &m_vDir);
	*vStartPos += -vLook * 1.f;
	vStartPos->y += 0.65f;

	m_pTransformCom->Set_Position(*vStartPos);

	m_pTransformCom->Scaling(0.04f, 0.04f, 0.04f);
	m_pTransformCom->Set_Degree(*vRot);
	m_pTransformCom->Rotation_X(90.f);
	m_pTransformCom->Rotation_Y(180.f);

	return NOERROR;
}

_int CDragonArrow::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		dynamic_cast<CDragonKiller*>(m_pDragonKiller)->Set_DeadWeapon();
		return 1;
	}
	m_fTimeDelta = fTimeDelta;
	m_fTimeShader += fTimeDelta;

	m_pTransformCom->Move_ToDirection(m_vDir, 9.f, fTimeDelta);

	_vec3 fDist = *D3DXVec3Normalize(&m_vDir, &m_vDir) * 4.f * fTimeDelta;

	m_fDist += D3DXVec3Length(&fDist);

	Collision_ToTarget();

	if (m_fDist > 20.f)
	{
		m_bIsDead = true;
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CDragonArrow::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CDragonArrow::Render_GameObject(void)
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

HRESULT CDragonArrow::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Mesh_Arrow");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_OBB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.m_pMaterialCom
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

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

HRESULT CDragonArrow::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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

void CDragonArrow::Collision_ToTarget()
{
	if (m_pWave_Manager->Get_WaveList()->empty())
		return;
	auto& iter = m_pWave_Manager->Get_WaveList()->begin();
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
			if (true == m_pColliderCom->Collision_OBB((CCollider*)pCollider))
			{
				dynamic_cast<CMonster*>((*iter))->Set_Hit(1.f);
				//m_bIsDead = true;
			}
		}
	}
}

CDragonArrow * CDragonArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 * vStartPos, _vec3* vDir, _vec3* vRot, CGameObject* pObject, SCENETYPE eType)
{
	CDragonArrow*	pInstance = new CDragonArrow(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vStartPos, vDir, vRot, pObject, eType)))
	{
		MSG_BOX("CrossBox Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CDragonArrow::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
