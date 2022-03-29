#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CPaticle : public CComponent
{

protected:
	explicit CPaticle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPaticle(const CPaticle & rhs);
	virtual ~CPaticle() = default;

public:
	HRESULT	Ready_Paticle(const _tchar* pFilePath);

public:
	virtual void Paticle_preRender();
	virtual void Paticle_Render();
	virtual void Paticle_Render(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name);
	virtual void Particle_PostRender();

	bool	isEmpty();
	bool	isDead();

	void	All_IsDead();

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
	virtual void Remove_DeadPaticle();

protected:
	_vec3						m_vOrigin;			// 시스템의 원천, 파티클이 시작하는곳
	BOUNDINGBOX					m_boundingbox;		// 파티클이 이동할수 있는 부피를 제한하는곳
	_float						m_fEmitRate;		// 시스템에 파티클이 추가되는 비율, 보통은 초당 파티클수로 기록
	_float						m_fSize;			// 시스템 내 모든 파티클의 크기 
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;
	LPDIRECT3DVERTEXBUFFER9		m_pVertex = nullptr;
	
	list<PATICLEATTRIBUTE>		m_PaticleList;		// 시스템 내 파티클 속성의 리스트
													// 여기서 갱신, 삭제 관리, 준비되면 리스트 노드의 일부를 버텍스 버퍼에 복사하고 파티클 드로잉과정을 거친다
													// 이어 다음 단계의 데이터를 복사하고 다시 파티클을 그리며, 모든 파티클을 그릴 때까지 이 과정을 반복.
	_uint						m_iMaxPaticle;		// 주어진 시간동안 만들어지는 최대 갯수

	DWORD						dwSize;				// 버텍스 버퍼가 보관할수 있는 파티클의 수
	DWORD						dwOffset;			// 파티클 시스템 랜더링에 사용
	DWORD						dwBatchSize;		//  이하동문

public:
	virtual CComponent* Clone(void) = 0;

protected:
	virtual _ulong Free(void);

};

END