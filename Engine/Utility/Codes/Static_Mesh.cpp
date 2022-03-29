#include "..\Headers\Static_Mesh.h"


USING(Engine)

CStatic_Mesh::CStatic_Mesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pAdjancey(nullptr)
	, m_pSubsets(nullptr)
	, m_dwNumSubsets(0)
	, m_pMesh(nullptr)
	, m_pSubsetInfo(nullptr)
	, m_ppTexture(nullptr)
	, m_vMin(0.f, 0.f, 0.f)
	, m_vMax(0.f, 0.f, 0.f)
{
}

CStatic_Mesh::CStatic_Mesh(const CStatic_Mesh & rhs)
	: CMesh(rhs)
	, m_pAdjancey(rhs.m_pAdjancey)
	, m_pSubsets(rhs.m_pSubsets)
	, m_dwNumSubsets(rhs.m_dwNumSubsets)
	, m_pMesh(rhs.m_pMesh)
	, m_pSubsetInfo(rhs.m_pSubsetInfo)
	, m_ppTexture(new MESHTEXTURE[m_dwNumSubsets])
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		m_ppTexture[i] = rhs.m_ppTexture[i];

		for (size_t j = 0; j < MESHTEXTURE::TEX_END; ++j)
		{
			if (nullptr != m_ppTexture[i].pTexture[j])
				m_ppTexture[i].pTexture[j]->AddRef();
		}
	}
	m_pMesh->AddRef();


}

HRESULT CStatic_Mesh::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	_tchar			szFullPath[256] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	HRESULT hr = (D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev, &m_pAdjancey,
		&m_pSubsets, nullptr, &m_dwNumSubsets, &m_pMesh));
	if (FAILED(hr))
		return E_FAIL;

	m_pSubsetInfo = new D3DXMATERIAL_DERIVED[m_dwNumSubsets];
	m_ppTexture = new MESHTEXTURE[m_dwNumSubsets];
	ZeroMemory(m_ppTexture, sizeof(MESHTEXTURE) * m_dwNumSubsets);

	_ulong dwVtxFVF = m_pMesh->GetFVF();

	if (false == (dwVtxFVF & D3DFVF_NORMAL))
	{
		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjancey->GetBufferPointer());
	}

	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		D3DXMATERIAL*	pSubset = (D3DXMATERIAL*)m_pSubsets->GetBufferPointer();

		m_pSubsetInfo[i].MatD3D = pSubset[i].MatD3D;
		m_pSubsetInfo[i].pTextureFilename = pSubset[i].pTextureFilename;

		_tchar		szFileName[128] = L"";
		lstrcpy(szFullPath, pFilePath);

		m_pSubsetInfo[i].isAlpha = isAlpha(pSubset[i].pTextureFilename, "A");

		MultiByteToWideChar(CP_ACP, 0, m_pSubsetInfo[i].pTextureFilename, strlen(m_pSubsetInfo[i].pTextureFilename), szFileName, 128);

		lstrcat(szFullPath, szFileName);

		if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED])))
			return E_FAIL;

		_tchar		szFileName_Uni[128] = L"";

		// For.Normal Texture
		lstrcpy(szFullPath, pFilePath);
		lstrcpy(szFileName, Convert_KeyWordTextureName(szFileName_Uni, m_pSubsetInfo[i].pTextureFilename, "N"));
		lstrcat(szFullPath, szFileName);
		D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTexture[i].pTexture[MESHTEXTURE::TEX_NORMAL]);

		// For.Specualr Texture
		lstrcpy(szFullPath, pFilePath);
		lstrcpy(szFileName, Convert_KeyWordTextureName(szFileName_Uni, m_pSubsetInfo[i].pTextureFilename, "S"));
		lstrcat(szFullPath, szFileName);
		D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTexture[i].pTexture[MESHTEXTURE::TEX_SPECULAR]);
	}

	D3DVERTEXELEMENT9		Vertex_Element[MAX_FVF_DECL_SIZE];
	m_pMesh->GetDeclaration(Vertex_Element);

	_ushort   wOffset = 0;

	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Vertex_Element[i].Usage)
		{
			wOffset = Vertex_Element[i].Offset;
			break;
		}
	}
	void*		pVertex = nullptr;

	m_pMesh->LockVertexBuffer(0, &pVertex);

	_ulong	dwNumVertices = m_pMesh->GetNumVertices();

	_ulong	dwMeshFVF = m_pMesh->GetFVF();

	_uint	iStride = D3DXGetFVFVertexSize(dwMeshFVF);

	D3DXComputeBoundingBox((_vec3*)((_byte*)pVertex + wOffset), dwNumVertices, iStride, &m_vMin, &m_vMax);
	
	return NOERROR;
}

void CStatic_Mesh::Render_Mesh(void)
{
	if (nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		m_pGraphicDev->SetMaterial(&m_pSubsetInfo[i].MatD3D);
		m_pGraphicDev->SetTexture(0, m_ppTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED]);

		if (true == m_pSubsetInfo[i].isAlpha)
		{
			m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x3f);
			m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			m_pMesh->DrawSubset(i);

			m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
		else
			m_pMesh->DrawSubset(i);
	}
}

void CStatic_Mesh::Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name)
{
	if (nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		pEffect->SetTexture(pConstant_Texture_Name, m_ppTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED]);
	
		pEffect->CommitChanges();

		if (true == m_pSubsetInfo[i].isAlpha)
		{
			//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			//m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x3f);
			//m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			m_pMesh->DrawSubset(i);

			//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
		else
			m_pMesh->DrawSubset(i);
	}
}

_bool CStatic_Mesh::isAlpha(const char * pFileName, const char * pKeyWord)
{
	_uint iLength = strlen(pFileName);

	for (size_t i = iLength - 1; i >= 0; --i)
	{
		if ('.' == pFileName[i])
		{
			if (*pKeyWord == pFileName[--i])
				return true;
			else
				break;
		}
	}
	return false;
}

const _tchar * CStatic_Mesh::Convert_KeyWordTextureName(_tchar* szFileName_Uni,const char * pFileName, const char * pKeyWord)
{
	char		szFileName[128] = "";
	char		szExtention[128] = "";

	_uint		iLength = strlen(pFileName);

	for (size_t i = iLength - 1; i >= 0; --i)
	{
		if ('.' == pFileName[i])
		{
			memcpy(szFileName, pFileName, sizeof(char) * (i - 1));
			strcat(szFileName, pKeyWord);

			memcpy(szExtention, pFileName + i, sizeof(char) * (iLength - i));
			strcat(szFileName, szExtention);

			break;
		}
	}

	MultiByteToWideChar(CP_ACP, 0, szFileName, strlen(szFileName)
		, szFileName_Uni, 128);

	return szFileName_Uni;
}

CComponent * CStatic_Mesh::Clone(void)
{
	return new CStatic_Mesh(*this);
}

CStatic_Mesh * CStatic_Mesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName)
{
	CStatic_Mesh *	pInstance = new CStatic_Mesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName)))
	{
		MessageBox(0, L"CStatic_Mesh Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CStatic_Mesh::Free(void)
{
	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		for (size_t j = 0; j < MESHTEXTURE::TEX_END; ++j)
		{
			Safe_Release(m_ppTexture[i].pTexture[j]);
		}
	}
	Safe_Delete_Array(m_ppTexture);

	Safe_Release(m_pMesh);

	return CMesh::Free();
}
