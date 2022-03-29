#pragma once

#include "Paticle.h"

BEGIN(Engine)

class DLL_EXPORT CBossPaticle : public CPaticle
{
private:
	explicit CBossPaticle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBossPaticle(const CBossPaticle& rhs);
	virtual ~CBossPaticle() = default;

public:
	HRESULT	Ready_SnowPaticle(const _tchar* pFilePath, _uint numPaticle);
	void Add_Paticle();
	void Reset();
	void Reset_Paticle(PATICLEATTRIBUTE* attribute);
	void Paticle_Update(const _float& fTimeDelta);

public:
	void Set_OriginPos(_vec3* vPos);
	void Set_Paticle();
	void Set_Ditection(_vec3* vDir);
	void Set_BossStage(_bool bStage) { m_bBossStage = bStage; }
private:
	_uint m_numPaticle = 0;
	_vec3 m_vDir = _vec3(0.f, 0.f, 0.f);
	_bool m_bBossStage = false;

public:
	virtual CComponent* Clone(void) final;
public:
	static CBossPaticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, _uint numPaticle);

private:
	virtual _ulong Free(void);

};

END

