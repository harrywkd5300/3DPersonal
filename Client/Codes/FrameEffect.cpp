#include "stdafx.h"
#include "..\Headers\FrameEffect.h"

#include "Component_Manager.h"

CFrameEffect::CFrameEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	//, m_pTextureCom(nullptr)
	, m_pShaderCom(nullptr)
{

}

HRESULT CFrameEffect::Ready_GameObject(_vec3* vPos, _vec3* vRot, _vec3* vDir)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Position(*vPos);

	m_pTransformCom->Scaling(1.5f, 1.5f, 0.5f);

	//m_pTransformCom->Rotation_Z(90.f);

	m_fRotaFirst = 0.f;
	_vec3 vLook = m_pTransformCom->Get_Look();

	m_pTransformCom->Get_LookDgreeY(m_fRotaFirst, &vLook, vDir);

	m_pTransformCom->Rotation_Y(m_fRotaFirst);

	return NOERROR;
}

_int CFrameEffect::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fTimeDelta += fTimeDelta;

	//m_pTransformCom->Rotation_X(10.f);


	// ºôº¸µå
	//_matrix			matView;
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixInverse(&matView, nullptr, &matView);
	//_vec3 vCamRight = matView.m[0];
	//_vec3 vCamUp = matView.m[1];
	//_vec3 vCamLook = matView.m[2];

	m_pTransformCom->Make_WorldMatrix();

	//m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_RIGHT][0], Engine::CTransform::INFO_RIGHT);
	//m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_UP][0], Engine::CTransform::INFO_UP);
	//m_pTransformCom->Set_MatrixRow((_vec3)&matView.m[Engine::CTransform::INFO_LOOK][0], Engine::CTransform::INFO_LOOK);
	//m_pTransformCom->Set_MatrixScale(_vec3(1.8f, 1.8f, 1.8f));
	//if (m_fDegree > 360.f)
	//	m_fDegree = 0.f;
	//++m_fDegree;
	//m_pTransformCom->BillBoard_RotationX(m_fDegree,fTimeDelta);

	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	_vec3 vCamPos = matView.m[3];
	m_fSortingDist = m_pTransformCom->Compute_Distance(&vCamPos);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CFrameEffect::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom
		|| nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	SetUp_OnContantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(3);

	// Rect
	m_pBufferCom->Render_Buffer();

	// Mesh
	//m_pBufferCom->Render_Mesh(pEffect, "g_BaseTexture");

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CFrameEffect::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	//// For.Buffers
	//pComponent = m_pBufferCom = (Engine::CStatic_Mesh*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Texture_FlameEffect");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	//m_pBufferCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Buffer_RectTex");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_FlameBall");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"Component_Shader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	//Component_Buffer_BulletTrail

	return NOERROR;
}

HRESULT CFrameEffect::SetUp_OnContantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iTexIdx);

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

	// For.Alpha
	pEffect->SetFloat("g_fAlpha", 1.0f);

	// For.Color
	pEffect->SetFloat("g_fColorG", 100.f);
	pEffect->SetFloat("g_fColorB", 100.f);
	pEffect->SetFloat("g_fColorR", 100.f);
	pEffect->SetFloat("g_fColorA", 1.f);


	Safe_Release(pEffect);

	return NOERROR;
}

void CFrameEffect::Set_Position(_vec3 * vPos)
{
	m_pTransformCom->Set_Position(*vPos);
}

CFrameEffect * CFrameEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, _vec3* vRot, _vec3* vDir)
{
	CFrameEffect *	pInstance = new CFrameEffect(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPos, vRot, vDir)))
	{
		MSG_BOX("CFrameEffect Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CFrameEffect::Free(void)
{
	_ulong		dwRefCnt = 0;


	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
