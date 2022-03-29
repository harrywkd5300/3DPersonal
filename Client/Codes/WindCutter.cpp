#include "stdafx.h"
#include "..\Headers\WindCutter.h"

#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Layer.h"

#include "input_Device.h"

#include "Player.h"
#include "boss.h"

WindCutter::WindCutter(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pPlayerTrans(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMaterialCom(nullptr)
	, m_fTimeDelta(0.f)
{
}

HRESULT WindCutter::Ready_GameObject(_vec3 * vStartPos, _vec3 * vLook, _vec3 * vEndPos, SCENETYPE eType)
{
	m_eType = eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	vStartPos->y += 2.f;

	m_pTransformCom->Set_Position(*vStartPos);

	m_vStartPos = *vStartPos;
	m_vEndPos = *vEndPos;
	m_vLook = *vLook; 

	m_vEndPos.y += 1.f;

	_float fDgree = 0.f;
	_vec3 vEndLook = m_vEndPos - m_vStartPos;

	m_pTransformCom->Get_LookDgreeY(fDgree, &m_pTransformCom->Get_Look(), &vEndLook);

	m_ColDgree.y = fDgree;

	m_pTransformCom->Set_DegreeY(fDgree);

	m_pTransformCom->Scaling(_vec3(0.01f, 0.02f, 0.3f));

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	//m_pTransformCom->Scaling(_vec3(0.05f, 0.03f, 0.8f));	HeadProjectTile

	//m_pTransformCom->Scaling(_vec3(0.05f, 0.005f, 0.01f)); // GrowBall

	return NOERROR;
}

_int WindCutter::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	m_fTimeDelta += fTimeDelta;

	m_pTransformCom->Rotation_Y(90.f);

	//if (m_bStart)
	//{
		_vec3 vDir = m_vEndPos - m_vStartPos;

		m_pTransformCom->Move_ToDirection(vDir, 10.f, fTimeDelta);

		_vec3 vGo = m_pTransformCom->Get_Position();

		_vec3 vLen = vGo - m_vStartPos;

		if (D3DXVec3Length(&vLen) > D3DXVec3Length(&vDir) + 4.f)
		{
			m_bIsDead = true;
		}
	//}
	//else
	//{
	//	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_B) & 0x80)
	//	{
	//		m_bStart = true;
	//	}
	//}

	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	_vec3 vCamPos = matView.m[3];
	m_fSortingDist = m_pTransformCom->Compute_Distance(&vCamPos);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	Collision_ToPlayer();

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

_int WindCutter::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void WindCutter::Render_GameObject(void)
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

	pEffect->BeginPass(2);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT WindCutter::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	//For.Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_WindCutter");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	////For.Collider
	//pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Buffer_Collider");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	//m_pColliderCom->AddRef();

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

HRESULT WindCutter::Ready_Collider()
{
	CComponent*			pComponent = nullptr;

	//For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_OBB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	return NOERROR;
}

HRESULT WindCutter::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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
	pEffect->SetFloat("g_fTimeDeltaU", 0.f);
	pEffect->SetFloat("g_fTimeDeltaV", m_fTimeDelta);

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
	// For.Color
	pEffect->SetFloat("g_fColorG", 15.f);
	pEffect->SetFloat("g_fColorB", 15.f);
	pEffect->SetFloat("g_fColorR", 15.f);
	pEffect->SetFloat("g_fColorA", 1.f);

	Safe_Release(pEffect);

	return NOERROR;
}

void WindCutter::Collision_ToPlayer()
{
	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	_vec3 vPos = m_pPlayerTrans->Get_Position();

	if (m_pTransformCom->Compute_Distance(&vPos) < 5.f)
	{
		CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
		if (nullptr == pCollider_Player)
			return;

		if (true == m_pColliderCom->Collision_OBB((CCollider*)pCollider_Player))
		{
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
			dynamic_cast<CPlayer*>(iter)->Set_Hit(true);
			dynamic_cast<CPlayer*>(iter)->Set_PlayerHitState(CPlayer::SMASH_HIT);
			m_bIsDead = true;
		}
	}
}

WindCutter * WindCutter::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 * vStartPos, _vec3 * vLook, _vec3 * vEndPos, SCENETYPE eType)
{
	WindCutter*	pInstance = new WindCutter(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vStartPos, vLook, vEndPos, eType)))
	{
		MSG_BOX("WindCutter Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong WindCutter::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pPlayerTrans);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
