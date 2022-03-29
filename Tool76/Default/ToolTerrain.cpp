#include "stdafx.h"
#include "ToolTerrain.h"

#include "Component_Manager.h"

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTexture(nullptr)
	, m_iTextureNum(1)
	, m_bViewWireFrame(false)
	, m_pShader(nullptr)
	, m_fBrushSize(5.f)
{
}

HRESULT CToolTerrain::Ready_GameObject()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Filter()))
		return E_FAIL;

	return NOERROR;
}

_int CToolTerrain::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	m_fTimeDelta = fTimeDelta;

	Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CToolTerrain::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bIsLastDead)
		return 1;

	return 0;
}

void CToolTerrain::Render_GameObject(void)
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
	if (m_bViewWireFrame)
		pEffect->BeginPass(1);
	else
		pEffect->BeginPass(2);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CToolTerrain::Ready_Component(void)
{
	CComponent*			pComponent = nullptr;
	// For.Material
	pComponent = m_pMaterialCom = (Engine::CMaterial*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Material");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Material", pComponent));
	m_pMaterialCom->AddRef();

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Transform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	/*create Picking*/
	pComponent = m_pPickingCom = (Engine::CPicking*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Picking");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Picking", pComponent));
	m_pPickingCom->AddRef();


	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CTerrain_Buffer*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Buffer_Terrain");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For. Texture
	pComponent = m_pTexture = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_BackLogo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTexture->AddRef();

	// For. Texture
	pComponent = m_pDestCom0 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_SnowBase");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture0", pComponent));
	m_pDestCom0->AddRef();

	// For. Texture
	pComponent = m_pDestCom1 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_Rocks");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture1", pComponent));
	m_pDestCom1->AddRef();

	// For. Texture
	pComponent = m_pDestCom2 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_Multi0");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture2", pComponent));
	m_pDestCom2->AddRef();

	// For.Texture_Filter
	//pComponent = m_pFilterCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_Filter");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Filter", pComponent));
	//m_pFilterCom->AddRef();

	// For. Texture
	pComponent = m_pTextureBrush = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Texture_Brush");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Brush", pComponent));
	m_pTextureBrush->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Renderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShader = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Shader_Terrain");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShader->AddRef();

	return NOERROR;
}

HRESULT CToolTerrain::Ready_Filter()
{
	_vec3 vPos = *m_pBufferCom->Get_BufferData();

	if (FAILED(D3DXCreateTexture(m_pGraphicDev, UINT(vPos.x), UINT(vPos.y), UINT(vPos.z), 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFilterTexture)))
		return E_FAIL;

	D3DLOCKED_RECT		Lock_Rect;
	ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

	m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

	for (size_t i = 0; i < vPos.y; ++i)
	{
		for (size_t j = 0; j < vPos.x; ++j)
		{
			_uint iIndex = i * _uint(vPos.y) + j;

			((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f);
		}
	}
	m_pFilterTexture->UnlockRect(0);

	return NOERROR;
}

HRESULT CToolTerrain::Ready_TextureCom(const _uint& iIdx, const _tchar * ComName)
{
	CComponent*			pComponent = nullptr;

	if (0 == iIdx)
	{
		// For. Texture
		pComponent = m_pDestCom0 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComName);
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Dest0", pComponent));
		m_pDestCom0->AddRef();
	}
	else if (1 == iIdx)
	{
		// For. Texture
		pComponent = m_pDestCom1 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComName);
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Dest1", pComponent));
		m_pDestCom1->AddRef();
	}
	else if (2 == iIdx)
	{
		// For. Texture
		pComponent = m_pDestCom2 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComName);
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Dest2", pComponent));
		m_pDestCom2->AddRef();
	}
	else if (3 == iIdx)
	{
		// For. Texture
		pComponent = m_pDestCom3 = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, ComName);
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Dest3", pComponent));
		m_pDestCom3->AddRef();
	}

	return NOERROR;
}

HRESULT CToolTerrain::Alpha_Texture_Init()
{
	_vec3 vPos = *m_pBufferCom->Get_BufferData();

	D3DLOCKED_RECT		Lock_Rect;
	ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

	m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

	for (size_t i = 0; i < vPos.y; ++i)
	{
		for (size_t j = 0; j < vPos.x; ++j)
		{
			_uint iIndex = i * _uint(vPos.y) + j;

			((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f);
		}
	}
	m_pFilterTexture->UnlockRect(0);

	return E_NOTIMPL;
}

HRESULT CToolTerrain::SetUp_OnContantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture");
	pEffect->SetTexture("g_FilterTexture", m_pFilterTexture);
	m_pDestCom0->SetUp_OnShader(pEffect, "g_DestTexture2");
	m_pDestCom1->SetUp_OnShader(pEffect, "g_DestTexture3");
	m_pDestCom2->SetUp_OnShader(pEffect, "g_DestTexture4");

	m_pTextureBrush->SetUp_OnShader(pEffect, "g_TowerTexture");

	if (!m_bViewBrush)
	{
		CWnd* pWnd = AfxGetMainWnd();
		HWND hWnd = pWnd->m_hWnd;


		_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
		vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);
		if (_vec3(-1.f, -1.f, -1.f) != vPickPos)
		{
			pEffect->SetVector("g_vBrushPos", &_vec4(vPickPos, 1.f));
			pEffect->SetFloat("g_fBrushSize", m_fBrushSize);
		}
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

void CToolTerrain::Change_TerrianBuffer()
{
	_ulong dwNum = 0;
	dwNum = Engine::Safe_Release(m_pBufferCom);

	Delete_Component(L"Com_Buffer");

	CComponent*			pComponent = nullptr;

	// For. Buffers
	pComponent = m_pBufferCom = (Engine::CTerrain_Buffer*)CComponent_Manager::GetInstance()->Clone_Component(TOOL_STATIC, L"Component_Buffer_Terrain");
	if (nullptr == pComponent)
		return;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	Alpha_Texture_Init();
}

void CToolTerrain::Set_Spletting(const _uint & iIdx)
{
	if (1 == iIdx)
	{
		CWnd* pWnd = AfxGetMainWnd();
		HWND hWnd = pWnd->m_hWnd;
		_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
		vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);

		if (_vec3(-1.f, -1.f, -1.f) == vPickPos)
			return;

		_vec3 vPos = *m_pBufferCom->Get_BufferData();

		D3DLOCKED_RECT		Lock_Rect;
		ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

		m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

		for (size_t i = 0; i < vPos.y; ++i)
		{
			for (size_t j = 0; j < vPos.x; ++j)
			{
				_uint iIndex = i * _uint(vPos.y) + j;
				const _vec3* vPixel = m_pBufferCom->Get_VertexPos();
				_vec3 vLeng = vPixel[iIndex] - vPickPos;
				_float fLength = D3DXVec3Length(&vLeng);

				if (fLength < (m_fBrushSize * 0.5f) * 0.4f)
				{
					((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f);
				}
			}
		}
		m_pFilterTexture->UnlockRect(0);
	}
	else if (2 == iIdx)
	{
		CWnd* pWnd = AfxGetMainWnd();
		HWND hWnd = pWnd->m_hWnd;
		_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
		vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);

		if (_vec3(-1.f, -1.f, -1.f) == vPickPos)
			return;

		_vec3 vPos = *m_pBufferCom->Get_BufferData();

		D3DLOCKED_RECT		Lock_Rect;
		ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

		m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

		for (size_t i = 0; i < vPos.y; ++i)
		{
			for (size_t j = 0; j < vPos.x; ++j)
			{
				_uint iIndex = i * _uint(vPos.y) + j;

				const _vec3* vPixel = m_pBufferCom->Get_VertexPos();
				_vec3 vLeng = vPixel[iIndex] - vPickPos;
				_float fLength = D3DXVec3Length(&vLeng);

				if (fLength < (m_fBrushSize * 0.5f) * 0.4f)
				{
					((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.f);
				}
			}
		}
		m_pFilterTexture->UnlockRect(0);
	}
	else if (3 == iIdx)
	{
		CWnd* pWnd = AfxGetMainWnd();
		HWND hWnd = pWnd->m_hWnd;
		_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
		vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);

		if (_vec3(-1.f, -1.f, -1.f) == vPickPos)
			return;

		_vec3 vPos = *m_pBufferCom->Get_BufferData();

		D3DLOCKED_RECT		Lock_Rect;
		ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

		m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

		for (size_t i = 0; i < vPos.y; ++i)
		{
			for (size_t j = 0; j < vPos.x; ++j)
			{
				_uint iIndex = i * _uint(vPos.y) + j;

				const _vec3* vPixel = m_pBufferCom->Get_VertexPos();

				_vec3 vLeng = vPixel[iIndex] - vPickPos;
				_float fLength = D3DXVec3Length(&vLeng);

				if (fLength < (m_fBrushSize * 0.5f) * 0.4f )
				{
					((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.f);
				}
			}
		}
		m_pFilterTexture->UnlockRect(0);
	}
	else
	{
		CWnd* pWnd = AfxGetMainWnd();
		HWND hWnd = pWnd->m_hWnd;
		_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
		vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);

		if (_vec3(-1.f, -1.f, -1.f) == vPickPos)
			return;

		_vec3 vPos = *m_pBufferCom->Get_BufferData();

		D3DLOCKED_RECT		Lock_Rect;
		ZeroMemory(&Lock_Rect, sizeof(D3DLOCKED_RECT));

		m_pFilterTexture->LockRect(0, &Lock_Rect, nullptr, 0);

		for (size_t i = 0; i < vPos.y; ++i)
		{
			for (size_t j = 0; j < vPos.x; ++j)
			{
				_uint iIndex = i * _uint(vPos.y) + j;

				const _vec3* vPixel = m_pBufferCom->Get_VertexPos();

				_vec3 vLeng = vPixel[iIndex] - vPickPos;
				_float fLength = D3DXVec3Length(&vLeng);

				if (fLength < (m_fBrushSize * 0.5f) * 0.4f)
				{
					((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f);
				}
			}
		}
		m_pFilterTexture->UnlockRect(0);
	}
}

void CToolTerrain::Make_Height()
{
	if (FAILED(D3DXSaveTextureToFile(L"../Bin/Tmp.jpg", D3DXIFF_JPG, m_pFilterTexture, nullptr)))
	{
		AfxMessageBox(L"Saved Failed!!");
		return;
	}
}

void CToolTerrain::Plus_Height(const _float& fSize, const _float& fMaxSize)
{
	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = pWnd->m_hWnd;
	_vec3 vPickPos = _vec3(0.f, 0.f, 0.f);
	vPickPos = m_pPickingCom->Picking_ToNaviBuffer(hWnd, BACKCX, BACKCY, (CVIBuffer*)m_pBufferCom, m_pMousePos.x, m_pMousePos.y, nullptr);

	if (_vec3(-1.f, -1.f, -1.f) == vPickPos)
		return;

	m_pBufferCom->HeightTerrain(&vPickPos, m_fBrushSize, fMaxSize, fSize, 3.f * m_fTimeDelta);
}

_vec3 * CToolTerrain::Get_TerrainBufferData()
{
	_vec3 vPos = *m_pBufferCom->Get_BufferData();
	return &vPos;
}

const _vec3 * CToolTerrain::Get_TerrainData()
{
	return m_pBufferCom->Get_VertexPos();
}

void CToolTerrain::Load_TerrainData(const _vec3 * vPosition)
{
	if (nullptr == vPosition)
		return;

	m_pBufferCom->LoadTerrain(vPosition);
}

CToolTerrain * CToolTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CToolTerrain *	pInstance = new CToolTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CToolTerrain Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CToolTerrain::Free()
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom );
	Engine::Safe_Release(m_pTexture);
	Engine::Safe_Release(m_pShader);
	Engine::Safe_Release(m_pMaterialCom);
	Engine::Safe_Release(m_pPickingCom);
	Engine::Safe_Release(m_pTextureBrush);
	//Engine::Safe_Release(m_pFilterCom);
	Engine::Safe_Release(m_pFilterTexture);


	Engine::Safe_Release(m_pDestCom0);
	Engine::Safe_Release(m_pDestCom1);
	Engine::Safe_Release(m_pDestCom2);


	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
