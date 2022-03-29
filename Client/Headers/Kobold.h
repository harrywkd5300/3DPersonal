#pragma once

#include "Monster.h"
#include "Defines.h"

class CWave_Manager;
class CKobold final : public CMonster
{
	enum KOBOLD_STATE {
		KOBOLD_FORWARD, KOBOLD_IDLE, KOBOLD_DEATH, KOBOLD_HURT, KOBOLD_SHOCK,
		KOBOLD_ATTACKSTART, KOBOLD_ATTACK0, KOBOLDSD_ATTACKSTART, KOBOLDSD_ATTACK0, KOBOLD_TURN, KOBOLD_END
	};

	enum  KOBOLD_ATTACK { KOBOLDATK_ATK0,
		KOBOLDATKSD_ATK0, KOBOLDATK_END };
private:
	explicit CKobold(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKobold() = default;

public:
	virtual HRESULT Ready_GameObject(const _uint& iIdx);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	HRESULT Ready_Mesh();
	HRESULT Ready_Collider();
	HRESULT Ready_Navigation(const _uint& iIdx);
	void	State_Update();
	void	Move_Partern(const _float& fTimeDelta);
	void	Attack(const _float& fTimeDelta);

	void	Collision_Monster();

private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	CTransform*			m_pTargetPlayer = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;

private:
	KOBOLD_STATE		m_eCurrState = KOBOLD_END;
	KOBOLD_STATE		m_eNewState = KOBOLD_END;

	KOBOLD_ATTACK		m_eAtkState = KOBOLDATK_END;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_uint				m_iNaviIdx = 1;

	// Dead
	//_float				m_fHp = 300.f;
	_uint				m_iPass = 1;
	_bool				m_bMinusHP = false;
	_float				m_fDeadTime = 0.f;

	_float				m_fAlpha = 1.f;

	_bool				m_bMotion = false;
	_bool				m_bFirst = false;
	_bool				m_bAttack = false;

	_bool				m_bAttackMove = false;
	// Attack0
	_bool				m_bAttackCoolTime0 = true;
	_float				m_fAttackCoolTime0 = 0.f;

	_float				m_fAttackStartTime0 = 0.f;

public:
	_bool		Get_View() { return m_bFirst; }
	void		Set_View(_bool	view) { m_bFirst = view; }
	//void		Set_MinusHp() { m_fHp = -1.f; }

public:
	static CKobold*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx);

private:
	virtual _ulong Free();
};

