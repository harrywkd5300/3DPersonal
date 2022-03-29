#pragma once

#include "Mesh.h"

BEGIN(Engine)

class DLL_EXPORT CStatic_Mesh final : public CMesh
{
private:
	explicit CStatic_Mesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;

public:
	const _vec3* Get_Min() const {
		return &m_vMin;
	}
	const _vec3* Get_Max() const {
		return &m_vMax;
	}
	LPD3DXMESH*	Get_Mesh() {
		return &m_pMesh;
	}


public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);
	virtual void Render_Mesh(void);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name);

private:
	LPD3DXBUFFER			m_pAdjancey    = nullptr;
	LPD3DXBUFFER			m_pSubsets	   = nullptr;
	_ulong					m_dwNumSubsets = 0;
	LPD3DXMESH				m_pMesh		   = nullptr;
	D3DXMATERIAL_DERIVED*	m_pSubsetInfo  = nullptr;
	MESHTEXTURE*			m_ppTexture	   = nullptr;
	D3DXVECTOR3				m_vMin, m_vMax;

private:
	_bool isAlpha(const char* pFileName, const char* pKeyWord);
	const _tchar* Convert_KeyWordTextureName(_tchar* szFileName_Uni, const char* pFileName, const char* pKeyWord);

public:
	virtual CComponent* Clone(void);
	static CStatic_Mesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
private:
	virtual _ulong Free(void);
};

END