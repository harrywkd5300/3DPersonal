#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CHierachy_Loader : public ID3DXAllocateHierarchy
{
public:
	CHierachy_Loader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath);
	~CHierachy_Loader() = default;

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,
		LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	const _tchar*			m_pPath = nullptr;

private:
	_bool		  isAlpha(const char * pFileName, const char* pKeyWord);
	void		  Convert_KeyWordTextureName(const char * pFileName, const char * pKeyWord, _tchar* szFullName);

private:
	HRESULT Allocate_Name(char** ppDestName, const char* const pName);
public:
	static CHierachy_Loader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
};

END