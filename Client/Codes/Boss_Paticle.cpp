#include "stdafx.h"
#include "..\Headers\Boss_Paticle.h"

#include "component_Manager.h"
#include "object_Manager.h"
#include "Layer.h"

#include "Boss.h"

CBoss_Paticle::CBoss_Paticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_pBossPaticleCom(nullptr)

{
}

HRESULT CBoss_Paticle::Ready_GameObject(CTransform* Target)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTargetTrans = Target;

	_vec3 vLook;
	D3DXVec3Normalize(&vLook, &m_pTargetTrans->Get_Look());
	_vec3 vMake = m_pTargetTrans->Get_Position() - vLook * 1.f;
	vMake.y += 3.f;

	m_pBossPaticleCom->Set_OriginPos(&vMake);
	m_pBossPaticleCom->Set_Ditection(&vLook);

	m_pBossPaticleCom->Set_Paticle();

	return NOERROR;
}

_int CBoss_Paticle::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bCrystalGo)
	{
		if (m_bCrystalGo2)
		{
			_vec3 vLook;
			D3DXVec3Normalize(&vLook, &m_pTargetTrans->Get_Look());
			_vec3 vMake = m_pTargetTrans->Get_Position() - vLook * 1.5f;
			vMake.y += 3.f;

			_vec3 vGo = m_vCrystal - vMake;

			m_pBossPaticleCom->Set_BossStage(true);

			m_pBossPaticleCom->Set_OriginPos(&vMake);
			m_pBossPaticleCom->Set_Ditection(&vGo);
		}
		else
		{
			_vec3 vLook;
			D3DXVec3Normalize(&vLook, &m_pTargetTrans->Get_Look());
			_vec3 vMake = m_pTargetTrans->Get_Position() + vLook * 1.5f;
			vMake.y += 2.f;

			_vec3 vGo = m_vCrystal - vMake;

			m_pBossPaticleCom->Set_BossStage(true);

			m_pBossPaticleCom->Set_OriginPos(&vMake);
			m_pBossPaticleCom->Set_Ditection(&vGo);
		}
	}
	else
	{
		_vec3 vLook;
		D3DXVec3Normalize(&vLook, &m_pTargetTrans->Get_Look());
		_vec3 vMake = m_pTargetTrans->Get_Position() - vLook * 1.f;
		vMake.y += 6.f;
		m_pBossPaticleCom->Set_OriginPos(&vMake);
		m_pBossPaticleCom->Set_Ditection(&vLook);
	}
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	_vec3 vCamPos = matView.m[3];
	m_fSortingDist = m_pTransformCom->Compute_Distance(&vCamPos);


	m_pBossPaticleCom->Paticle_Update(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

_int CBoss_Paticle::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CBoss_Paticle::Render_GameObject(void)
{
	if (nullptr == m_pBossPaticleCom || nullptr == m_pMaterialCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;


	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pBossPaticleCom->Paticle_Render(pEffect, "g_BaseTexture");
	
	//m_pBossPaticleCom->Paticle_Render();

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CBoss_Paticle::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Material");
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


	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Paticle
	pComponent = m_pBossPaticleCom = (Engine::CBossPaticle*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Boss_Paticle");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Particle", pComponent));
	m_pBossPaticleCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Shader_Particle");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	//if (nullptr == m_pTargetTrans)
	//{
	//	m_pTargetTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_Boss", 0, L"Com_Transform");
	//	m_pTargetTrans->AddRef();
	//}

	return NOERROR;
}

HRESULT CBoss_Paticle::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	//_matrix matWorld;
	//D3DXMatrixIdentity(&matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

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
	//pEffect->SetFloat("g_fAlpha", m_fAlpha);

	// pSize
	pEffect->SetFloat("g_fSize", 3.f);

	Safe_Release(pEffect);

	return NOERROR;
}

void CBoss_Paticle::Set_BossPaticle()
{
}

void CBoss_Paticle::Set_DeadPaticle()
{
	m_pBossPaticleCom->Reset();
}

CBoss_Paticle * CBoss_Paticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* Target)
{
	CBoss_Paticle*	pInstance = new CBoss_Paticle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(Target)))
	{
		MSG_BOX("CBoss_Paticle Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CBoss_Paticle::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pBossPaticleCom);
	//Engine::Safe_Release(m_pTargetTrans);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
