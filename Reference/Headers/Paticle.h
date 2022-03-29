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
	_vec3						m_vOrigin;			// �ý����� ��õ, ��ƼŬ�� �����ϴ°�
	BOUNDINGBOX					m_boundingbox;		// ��ƼŬ�� �̵��Ҽ� �ִ� ���Ǹ� �����ϴ°�
	_float						m_fEmitRate;		// �ý��ۿ� ��ƼŬ�� �߰��Ǵ� ����, ������ �ʴ� ��ƼŬ���� ���
	_float						m_fSize;			// �ý��� �� ��� ��ƼŬ�� ũ�� 
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;
	LPDIRECT3DVERTEXBUFFER9		m_pVertex = nullptr;
	
	list<PATICLEATTRIBUTE>		m_PaticleList;		// �ý��� �� ��ƼŬ �Ӽ��� ����Ʈ
													// ���⼭ ����, ���� ����, �غ�Ǹ� ����Ʈ ����� �Ϻθ� ���ؽ� ���ۿ� �����ϰ� ��ƼŬ ����װ����� ��ģ��
													// �̾� ���� �ܰ��� �����͸� �����ϰ� �ٽ� ��ƼŬ�� �׸���, ��� ��ƼŬ�� �׸� ������ �� ������ �ݺ�.
	_uint						m_iMaxPaticle;		// �־��� �ð����� ��������� �ִ� ����

	DWORD						dwSize;				// ���ؽ� ���۰� �����Ҽ� �ִ� ��ƼŬ�� ��
	DWORD						dwOffset;			// ��ƼŬ �ý��� �������� ���
	DWORD						dwBatchSize;		//  ���ϵ���

public:
	virtual CComponent* Clone(void) = 0;

protected:
	virtual _ulong Free(void);

};

END