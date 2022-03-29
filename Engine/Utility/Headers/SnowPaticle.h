#pragma once

#include "Paticle.h"

BEGIN(Engine)

class DLL_EXPORT CSnowPaticle : public CPaticle
{
private:
	explicit CSnowPaticle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSnowPaticle(const CSnowPaticle& rhs);
	virtual ~CSnowPaticle() = default;

public:
	HRESULT	Ready_SnowPaticle(const _tchar* pFilePath, _uint numPaticle);
	void Add_Paticle();
	void Reset();
	void Reset_Paticle(PATICLEATTRIBUTE* attribute);
	void Paticle_Update(const _float& fTimeDelta);

public:
	void Set_BoundingBox(_vec3* vMin, _vec3* vMax);
	void Set_Paticle();

private:
	_uint m_numPaticle = 0;

public:
	virtual CComponent* Clone(void) final;
public:
	static CSnowPaticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, _uint numPaticle);

private:
	virtual _ulong Free(void);

};

END