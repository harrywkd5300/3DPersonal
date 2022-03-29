#include "stdafx.h"
#include "..\Headers\BossTerrain.h"

#include "Component_Manager.h"
#include "object_Manager.h"

CBossTerrain::CBossTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pShader(nullptr)
	, m_pTextureTowerRangeCom(nullptr)
	, m_bTowerRadius(false)
	, m_pDestCom0(nullptr)
	, m_pDestCom1(nullptr)
	, m_pDestCom2(nullptr)
	, m_pDestCom3(nullptr)
	, m_pFilterTexture(nullptr)
{

}

HRESULT CBossTerrain::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Height()))
		return E_FAIL;


	return NOERROR;
}

_int CBossTerrain::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pTargetBufferCom)
	{
		m_pTargetBufferCom = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		m_pTargetBufferCom->AddRef();
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pTransformCom->Make_WorldMatrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

void CBossTerrain::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom
		|| nullptr == m_pShader)
		return;

	LPD3DXEFFECT	pEffect = m_pShader->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	SetUp_OnContantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(2);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

void CBossTerrain::Set_TowerRadius()
{
	if (m_bTowerRadius)
		m_bTowerRadius = false;
	else
		m_bTowerRadius = true;
}

HRESULT CBossTerrain::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Material");
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
	pComponent = m_pBufferCom = (Engine::CTerrain_Buffer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Buffer_Terrain");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	//// For.Texture
	//pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_Terrain");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	//m_pTextureCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_Floor1_d");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Texture
	pComponent = m_pTextureTowerRangeCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_TowerRadius");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_TowerRadus", pComponent));
	m_pTextureTowerRangeCom->AddRef();

	// For. Texture
	pComponent = m_pDestCom0 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_SnowBase");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture0", pComponent));
	m_pDestCom0->AddRef();

	// For. Texture
	pComponent = m_pDestCom1 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_Rocks");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture1", pComponent));
	m_pDestCom1->AddRef();

	// For. Texture
	pComponent = m_pDestCom2 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Texture_Multi0");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture2", pComponent));
	m_pDestCom2->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShader = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_BOSS, L"Component_Shader_Terrain");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShader->AddRef();

	return NOERROR;
}

HRESULT CBossTerrain::Ready_Height()
{
	//HANDLE hFile = CreateFile(L"../Bin/Data/MapHeight00.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
	//	, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hFile = CreateFile(L"../Bin/Data/BossHeight.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Load Failed!!");
		return E_FAIL;
	}

	DWORD dwByte = 0;

	_vec3 vecFilter;

	ReadFile(hFile, &vecFilter, sizeof(_vec3), &dwByte, nullptr);

	m_pVertexPos = new _vec3[vecFilter.x * vecFilter.y];

	for (size_t i = 0; i < vecFilter.y; ++i)
	{
		for (size_t j = 0; j < vecFilter.x; ++j)
		{
			_uint iIndex = i * _uint(vecFilter.y) + j;

			ReadFile(hFile, &m_pVertexPos[iIndex], sizeof(_vec3), &dwByte, nullptr);
		}
	}

	m_pBufferCom->LoadTerrain(m_pVertexPos);

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CBossTerrain::SetUp_OnContantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");
	pEffect->SetTexture("g_FilterTexture", m_pFilterTexture);
	m_pDestCom0->SetUp_OnShader(pEffect, "g_DestTexture2");
	m_pDestCom1->SetUp_OnShader(pEffect, "g_DestTexture3");
	m_pDestCom2->SetUp_OnShader(pEffect, "g_DestTexture4");

	if (m_bTowerRadius)
	{
		_vec3 vPos = m_pTargetBufferCom->Get_Position();
		pEffect->SetVector("g_vBrushPos", &_vec4(vPos, 1.f));
		pEffect->SetFloat("g_fBrushSize", 4.f);
		m_pTextureTowerRangeCom->SetUp_OnShader(pEffect, "g_TowerTexture");
	}
	else
	{
		_vec3 vPos = _vec3(0.f, 0.f, 0.f);
		pEffect->SetVector("g_vBrushPos", &_vec4(vPos, 1.f));
		pEffect->SetFloat("g_fBrushSize", 0.f);
	}

	// For.LightInfo
	D3DLIGHT9			LightInfo;
	m_pGraphicDev->GetLight(0, &LightInfo);
	pEffect->SetVector("g_vLight_Direction", &_vec4(LightInfo.Direction, 0.f));
	pEffect->SetVector("g_vLight_Diffuse", (_vec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLight_Ambient", (_vec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLight_Specular", (_vec4*)&LightInfo.Specular);

	// For.MaterialInfo
	D3DMATERIAL9		MtrlInfo;
	m_pMaterialCom->Get_MaterialInfo(&MtrlInfo);

	pEffect->SetVector("g_vMaterial_Diffuse", (_vec4*)&MtrlInfo.Diffuse);
	pEffect->SetVector("g_vMaterial_Ambient", (_vec4*)&MtrlInfo.Ambient);
	pEffect->SetVector("g_vMaterial_Specular", (_vec4*)&MtrlInfo.Specular);

	// For.Camera	
	D3DXMatrixInverse(&matView, nullptr, &matView);

	pEffect->SetVector("g_vCamera_Position", (_vec4*)&matView.m[3][0]);
	pEffect->SetFloat("g_fPower", MtrlInfo.Power);

	Safe_Release(pEffect);

	return NOERROR;
}

CBossTerrain * CBossTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossTerrain *	pInstance = new CBossTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CBossTerrain Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CBossTerrain::Free(void)
{
	_ulong		dwRefCnt = 0;


	Engine::Safe_Release(m_pShader);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pTextureTowerRangeCom);
	Engine::Safe_Release(m_pTargetBufferCom);


	Engine::Safe_Release(m_pFilterTexture);
	Engine::Safe_Release(m_pDestCom0);
	Engine::Safe_Release(m_pDestCom1);
	Engine::Safe_Release(m_pDestCom2);
	Engine::Safe_Delete_Array(m_pVertexPos);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
