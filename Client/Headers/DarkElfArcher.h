#pragma once

#include "Monster.h"
#include "Defines.h"

class CWave_Manager;
class CDarkElfArcher final : public CMonster
{
	enum ARCHER_STATE {
		ARCHER_FORWARD, ARCHER_IDLE, ARCHER_DEATH, ARCHER_HURT, ARCHER_SHOCK,
		ARCHER_ATTACK1, ARCHER_ATTACK2,ARCHER_LEFT, ARCHER_END
	};

	enum  ARCHER_ATTACK { ARCHERATK_ATK0, ARCHERATK_SHIELD,ARCHERATK_END };
private:
	explicit CDarkElfArcher(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDarkElfArcher() = default;

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
	ARCHER_STATE		m_eCurrState = ARCHER_END;
	ARCHER_STATE		m_eNewState = ARCHER_END;

	ARCHER_ATTACK		m_eAtkState = ARCHERATK_END;

private:
	CTransform*			m_pTargetPlayer = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_uint				m_iNaviIdx = 1;

	// Dead
	_uint				m_iPass = 1;
	_bool				m_bMinusHP = false;
	_float				m_fDeadTime = 0.f;

	_float				m_fAlpha = 1.f;

	_bool				m_bMotion = false;
	_bool				m_bFirst = false;
	_bool				m_bAttack = false;

	_bool				m_bAttackMove = false;

	// Attack1
	_bool				m_bAttackCoolTime0 = true;
	_float				m_fAttackCoolTime0 = 0.f;

	_float				m_fAttackStartTime0 = 0.f;

public:
	_bool		Get_View() { return m_bFirst; }
	void		Set_View(_bool	view) { m_bFirst = view; }
	//void		Set_MinusHp() { m_fHp = -1.f; }
	void		Set_DeadShield() { m_pShield = nullptr; }

public:
	static CDarkElfArcher*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iIdx);

private:
	virtual _ulong Free();
};

