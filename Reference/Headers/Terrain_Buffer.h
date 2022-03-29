#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class DLL_EXPORT CTerrain_Buffer final : public CVIBuffer
{
private:
	explicit CTerrain_Buffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain_Buffer(const CTerrain_Buffer& rhs);
	virtual ~CTerrain_Buffer() = default;
public:
	HRESULT Ready_Buffer(const _tchar* pHeightMap, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _float & fInterval);
	void Render_Buffer(void);

public:
	void	HeightTerrain(const _vec3* vPickPos, const _float& fBrushSize, const _float& fMaxHeight, const _float& fSize, const _float fHeight = 1.f);
	void	LoadTerrain(const _vec3* vPosition);

public:
	virtual CComponent* Clone(void);
	static CTerrain_Buffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iVtxCntX, const _uint& iVtxCntZ, const _float& fInterval = 1.0f, const _tchar* pHeightMap = nullptr);
private:
	virtual _ulong Free(void);

};

END