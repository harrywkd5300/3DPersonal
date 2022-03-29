#pragma once

#include "Monster.h"
#include "Defines.h"

class CWave_Manager;
class COrcBruiser final : public CMonster
{
	enum ORGE_STATE {
		ORGE_FORWARD, ORGE_IDLE, ORGE_DEATH, ORGE_SHOCK,
		ORGE_ATTACK1, ORGE_ATTACK2, ORGESD_ATTACK1, ORGESD_ATTACK2, ORGE_LEFT, ORGE_END
	};

	enum  ORGE_ATTACK { ORGEATK_ATK0, ORGEATK_ATK1, ORGEATKSD_ATK0, ORGEATKSD_ATK1, ORGEATK_END };

private:
	explicit COrcBruiser(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrcBruiser() = default;

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
	void	CollTime(const _float& fTimeDelta);

	void	Collision_Monster();

private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	ORGE_STATE		m_eCurrState = ORGE_END;
	ORGE_STATE		m_eNewState = ORGE_END;

	ORGE_ATTACK		m_eAtkState = ORGEATK_END;

private:
	CTransform*			m_pTargetPlayer = nullptr;
	CCollider*			m_pMyCollider = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_uint				m_iNaviIdx = 1;

	// Dead
	_uint				m_iPass = 1;

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

	// Attack1
	_bool				m_bAttackCoolTime1 = true;
	_float				m_fAttackCoolTime1 = 0.f;

	_float				m_fAttackStartTime1 = 0.f;

public:
	_bool		Get_View() { return m_bFirst; }
	void		Set_View(_bool	view) { m_bFirst = view; }

public:
	static COrcBruiser*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx);

private:
	virtual _ulong Free();
};

