#pragma once

#include "Component.h"

BEGIN(Engine)

class  DLL_EXPORT CEffect : public CComponent
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	HRESULT	Ready_Effect(const _tchar* pFilePath);


public:   // In.Function
	DWORD FtoDw(float f) { return *((DWORD*)&f); }
	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound)
			return lowBound;

		float f = (rand() % 1000) * 0.001f;

		return (f * (highBound - lowBound)) + lowBound;
	}
	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max)
	{
		out->x = GetRandomFloat(min->x, max->x);
		out->y = GetRandomFloat(min->y, max->y);
		out->z = GetRandomFloat(min->z, max->z);
	}


protected:
	_vec3		m_vPosition = _vec3(0.f, 0.f, 0.f);
	_vec3		m_vScale = _vec3(0.f, 0.f, 0.f);
	_vec3		m_vRotation = _vec3(0.f, 0.f, 0.f);
	_vec3		m_vDirection = _vec3(0.f, 0.f, 0.f);

	_float		m_fMin_Speed = 0.f;
	_float		m_fMax_Speed = 0.f;

	_float		m_fFrame_Speed = 0.f;

	_float		m_fU = 0.f;
	_float		m_fV = 0.f;

	_float		m_fSprite_Cnt = 0.f;

	_float		m_fAlpha = 1.f;

	_float		m_fColorR = 1.f;
	_float		m_fColorG = 1.f;
	_float		m_fColorB = 1.f;
	_float		m_fColorA = 1.f;


public:
	virtual CComponent* Clone(void) = 0;

protected:
	virtual _ulong Free(void);
};

END