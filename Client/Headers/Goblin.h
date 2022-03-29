#pragma once

#include "Monster.h"
#include "Defines.h"

class CWave_Manager;
class CGoblin final : public CMonster
{
	enum GOBLIIN_STATE {
		GOBLIIN_FORWARD, GOBLIIN_IDLE, GOBLIIN_DEATH, GOBLIIN_HURT, GOBLIIN_SHOCK,
		GOBLIIN_ATTACK1, GOBLIIN_ATTACK2, GOBLIIN_ATTACK3, GOBLIIN_CHEER, GOBLIIN_LEFT, GOBLIIN_END };

	enum  GOBLIIN_ATTACK { GOBLIINATK_ATK0, GOBLIINATK_ATK1, GOBLIINATK_SHIELD, GOBLIINATK_END };
private:
	explicit CGoblin(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGoblin() = default;

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
	GOBLIIN_STATE		m_eCurrState = GOBLIIN_END;
	GOBLIIN_STATE		m_eNewState = GOBLIIN_END;

	GOBLIIN_ATTACK		m_eAtkState = GOBLIINATK_END;

private:
	CTransform*			m_pTargetPlayer = nullptr;
	//CGameObject*		m_pShield		= nullptr;

	CCollider*			m_pMyCollider = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_bool				m_bMotion = false;
	_bool				m_bFirst = false;
	_bool				m_bAttack = false;

	_uint				m_iNaviIdx = 1;

	// Dead
	_uint				m_iPass = 1;
	_bool				m_bMinusHP = false;
	_float				m_fDeadTime = 0.f;

	_float				m_fAlpha = 1.f;

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
	static CGoblin*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx);

private:
	virtual _ulong Free();
};

