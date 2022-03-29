#include "stdafx.h"
#include "..\Headers\Fire_Explosion.h"

#include "component_Manager.h"
#include "Object_Manager.h"

CFire_Explosion::CFire_Explosion(LPDIRECT3DDEVICE9 pGraphicDev)
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

HRESULT CFire_Explosion::Ready_GameObject(_vec3 * vStartPos, _vec3 * vScale, const _uint& iIdx, SCENETYPE g_eType)
{
	eType = g_eType;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_iMaxIdx = iIdx-1;

	vStartPos->y += 0.4f;

	m_pTransformCom->Set_Position(*vStartPos);
	m_vScale = *vScale;

	return NOERROR;
}

_int CFire_Explosion::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	m_fTimeDelta += fTimeDelta;

	if (m_iIdx > m_iMaxIdx)
	{
		m_iIdx = m_iMaxIdx;
		m_bIsDead = true;
	}
	else
	{
		++m_iIdx;
	}

	// ºôº¸µå
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	_vec3 vCamRight = matView.m[0];
	_vec3 vCamUp = matView.m[1];
	_vec3 vCamLook = matView.m[2];

	_vec3 vCamPos = matView.m[3];
	m_fSortingDist = m_pTransformCom->Compute_Distance(&vCamPos);

	m_pTransformCom->Make_WorldMatrix();

	m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_RIGHT][0], Engine::CTransform::INFO_RIGHT);
	m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_UP][0], Engine::CTransform::INFO_UP);
	m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_LOOK][0], Engine::CTransform::INFO_LOOK);
	m_pTransformCom->Set_MatrixScale(m_vScale);

	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

_int CFire_Explosion::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return CGameObject::LastUpdate_GameObject(fTimeDelta);
}

void CFire_Explosion::Render_GameObject(void)
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

	pEffect->BeginPass(1);

	// Rect
	m_pBufferCom->Render_Buffer();

	//m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CFire_Explosion::Ready_Component()
{
	CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Buffer_RectTex");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_Explosion");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(eType, L"Component_Buffer_Collider");
	if (nullptr == pComponent)
		return E_FAIL;

	_vec3 vMin = _vec3(-1.f, -1.f, -1.f);
	_vec3 vMax = _vec3(1.f, 1.f, 1.f);

	m_pColliderCom->Set_ColliderSetting(CCollider::TYPE_AABB, &vMin, &vMax, m_pTransformCom, &_vec3(0.f, 0.f, 0.f));
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.m_pMaterialCom
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(eType, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(eType, L"Component_Shader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	return NOERROR;
}

HRESULT CFire_Explosion::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iIdx);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//_float	TimeDelta = m_fTimeDelta;
	// TimeDelta
	pEffect->SetFloat("g_fTimeDeltaU", 0.f);
	pEffect->SetFloat("g_fTimeDeltaV", 0.f);

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
	pEffect->SetFloat("g_fAlpha", 3.f);
	// For.Color
	pEffect->SetFloat("g_fColorG", 20.f);
	pEffect->SetFloat("g_fColorB", 20.f);
	pEffect->SetFloat("g_fColorR", 20.f);
	pEffect->SetFloat("g_fColorA", 1.f);


	Safe_Release(pEffect);

	return NOERROR;
}

void CFire_Explosion::Collision_ToPlayer()
{
}

CFire_Explosion * CFire_Explosion::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 * vStartPos, _vec3 * vScale, const _uint& iIdx, SCENETYPE eType)
{
	CFire_Explosion*	pInstance = new CFire_Explosion(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vStartPos, vScale, iIdx, eType)))
	{
		MSG_BOX("CFire_Explosion Created Failed!!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CFire_Explosion::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pColliderCom);
	Engine::Safe_Release(m_pPlayerTrans);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::Safe_Release(m_pTextureCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
