#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CAnimation_Ctrl : public CBase
{
private:
	explicit CAnimation_Ctrl(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl);
	explicit CAnimation_Ctrl(const CAnimation_Ctrl& rhs);
	virtual ~CAnimation_Ctrl() = default;

public:
	HRESULT	Ready_Animation_Ctrl();
	HRESULT	Set_AnimationSet(const _uint& iAni_idx);
	void	Play_Animation(const _float& fDeltaTime, const _float & fAniSpeed = 1.f);

	_bool	End_Animation(const _double& Delay);

	void	Set_Animation();


private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
	LPD3DXANIMATIONCONTROLLER		m_pAnimation_Ctrl = nullptr;

	_uint							m_iCurrentTrack = 0;
	_uint							m_iNewTrack = 1;      // 새로운 트랙을 만들어 관리한다.
	_float							m_fTimeAcc = 0.f;

	_uint							m_iMaxNumAniSet = 0;
	_uint							m_iOldAniSetIdx = 0;

	_double							m_dwPeriod = 0;


public:
	static CAnimation_Ctrl*	Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl);
	static CAnimation_Ctrl* Create(const CAnimation_Ctrl& rhs);

private:
	virtual _ulong Free();
};

END