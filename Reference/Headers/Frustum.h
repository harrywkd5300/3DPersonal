#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CFrustum final : public CComponent
{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CFrustum(const CFrustum& rhs);
	virtual ~CFrustum() = default;

public:
	HRESULT Ready_Frustum(void);
	_bool isIn_Frustum(const _vec3* pPositionInWorldSpace, _float fRadius);
private:
	_vec3				m_vPoint[8]; // 절두체의 엳럽개 꼭치지ㅓㅁ을 읨히ㅏㄴ다.

private:

	void SetUp_PointsInProjection(void);

public:
	static CFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);

private:
	virtual _ulong Free(void);
};

END