#include "stdafx.h"
#include "..\Headers\BaseObj.h"

#include "component_Manager.h"
#include "object_manager.h"
#include "CollisionMgr.h"
#include "layer.h"
#include "Player.h"
#include "Arrow.h"

#include "SnowmanBoss.h"
#include "Management.h"
#include "Scene_Stage.h"
#include "Boss.h"

CBaseObj::CBaseObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr), m_pRendererCom(nullptr)
	, m_pBufferCom(nullptr), m_pColliderCom(nullptr)
	, m_pFrustum(nullptr), m_pMaterialCom(nullptr), m_pPlayerTrans(nullptr), m_pShaderCom(nullptr)
	, m_pObjectData(nullptr), m_vRota(0.f, 0.f, 0.f), m_isDraw(true), m_fFrustumSize(2.f)
{

}

HRESULT CBaseObj::Ready_GameObject(const _tchar * FullPath, const _tchar * FileName, const _tchar * ComPath, const _vec3 & vMakeScale, const _vec3 & vMakePos, const _vec3 & vMakeRot, SCENETYPE eType)
{
	m_pObjectData = new OBJECT;
	ZeroMemory(m_pObjectData, sizeof(OBJECT));

	lstrcpy(m_pObjectData->FullPath, FullPath);
	lstrcpy(m_pObjectData->FileName, FileName);
	lstrcpy(m_pObjectData->ComName, ComPath);

	m_eType = eType;
	m_vRota = vMakeRot;

	m_vPos = vMakePos;

	if (FAILED(Ready_MeshComponent(ComPath)))
		return E_FAIL;
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(vMakeScale);
	m_pTransformCom->Set_Position(vMakePos);
	m_pTransformCom->Set_Degree(vMakeRot);

	if (!lstrcmp(m_pObjectData->FileName, L"Church.X") || !lstrcmp(m_pObjectData->FileName, L"OldTrimLong.X"))
		m_fFrustumSize = 100.f;
	else if (!lstrcmp(m_pObjectData->FileName, L"magusQuartersRoof_SM.X"))
		m_fFrustumSize = 30.f;
	else
		m_fFrustumSize = 8.f;

	return NOERROR;
}

_int CBaseObj::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
	{
		return 1;
	}
	m_pTransformCom->Make_WorldMatrix();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	const _vec3 vPosition = m_pTransformCom->Get_Position();

	if (nullptr != m_pFrustum)
		m_isDraw = m_pFrustum->isIn_Frustum(&vPosition, m_fFrustumSize);

	if (m_bAlphaStart)
	{
		CGameObject* pObject = CObject_Manager::GetInstance()->Find_Layer(SCENE_STAGE, L"Layer_SnowBoss")->GetObjList()->front();
		if (m_fAlpha > 0.3f)
		{
			m_fAlpha -= 0.1f * fTimeDelta;
			dynamic_cast<SnowmanBoss*>(pObject)->Set_View(true);
			dynamic_cast<SnowmanBoss*>(pObject)->Set_OpenView(true);
		}
		else
		{
			m_bIsDead = true;
			CGameObject* pBoss = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Boss")->GetObjList()->front();
			dynamic_cast<CBoss*>(pBoss)->Set_StartSnowMan(false);
			dynamic_cast<CBoss*>(pBoss)->Set_StartBoss(true);
		}
	}

	if (nullptr != m_pRendererCom && true == m_isDraw)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CBaseObj::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return 0;
}

void CBaseObj::Render_GameObject(void)
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

	if(!m_bAlphaStart)
		pEffect->BeginPass(1);
	else
		pEffect->BeginPass(2);

	m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CBaseObj::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
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


	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CBaseObj::Ready_Component(void)
{
	CComponent*			pComponent = nullptr;
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

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, m_pBufferCom->Get_Min(), m_pBufferCom->Get_Max(), m_pTransformCom, &m_vRota);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Frustum
	pComponent = m_pFrustum = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Frustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Frustum", pComponent));
	m_pFrustum->AddRef();

	// For.m_pMaterialCom
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, L"Component_Shader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	return NOERROR;
}

void CBaseObj::Collision_ToObject(void)
{
	CComponent*		pCollider_Player = CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Collider");
	if (nullptr == pCollider_Player)
		return;

	if (nullptr == m_pPlayerTrans)
	{
		m_pPlayerTrans = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pPlayerTrans->AddRef();
	}

	_vec3 vPos = m_pPlayerTrans->Get_Position();

	m_pColliderCom->Collision_AABB((CCollider*)pCollider_Player);

	CLayer*	pLayer = CObject_Manager::GetInstance()->Find_Layer(m_eType, L"Layer_Arrow");
	if (nullptr == pLayer)
		return;
	if (pLayer->GetObjList()->empty())
		return;

	auto& iter = pLayer->GetObjList()->front();

	CComponent*		pCollider_Weapon = dynamic_cast<CArrow*>(iter)->Get_Component(L"Com_Collider");
	if (nullptr == pCollider_Weapon)
		return;
	m_pColliderCom->Collision_AABB((CCollider*)pCollider_Weapon);

}

HRESULT CBaseObj::Ready_MeshComponent(const _tchar * ComPath)
{
	CComponent*			pComponent = nullptr;

	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(m_eType, ComPath);
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	return NOERROR;
}

CBaseObj * CBaseObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * FullPath, const _tchar * FileName, const _tchar * ComPath, const _vec3 & vMakeScale, const _vec3 & vMakePos, const _vec3 & vMakeRot, SCENETYPE eType)
{
	CBaseObj*	pInstance = new CBaseObj(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(FullPath, FileName, ComPath, vMakeScale, vMakePos, vMakeRot, eType)))
	{
		MSG_BOX("CBaseObj Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CBaseObj::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pFrustum);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPlayerTrans);
	Engine::Safe_Release(m_pShaderCom);

	Engine::Safe_Delete(m_pObjectData);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
