#pragma once

#include "Monster.h"
#include "Defines.h"


class SnowmanBoss final : public CMonster
{
	enum SNOWMAN_STATE { SNOWMAN_FORWARD, SNOWMAN_IDLE, SNOWMAN_DEATH, 
						 SNOWMAN_ATTACK1, SNOWMAN_ATTACK2, SNOWMAN_END  };

	enum SNOWMAN_ATTACK { SNOWMAATK_JUMP, SNOWMANATK_ATK, SNOWMANATK_END };

private:
	explicit SnowmanBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~SnowmanBoss() = default;

public:
	virtual HRESULT Ready_GameObject(SCENETYPE eType);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	HRESULT Ready_Mesh();
	HRESULT Ready_Collider();
	void	State_Update();
	void	Move_Partern(const _float& fTimeDelta);
	void	Attack(const _float& fTimeDelta);
	void	CollTime(const _float& fTimeDelta);

private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	SNOWMAN_STATE		m_eCurrState = SNOWMAN_END;
	SNOWMAN_STATE		m_eNewState = SNOWMAN_END;

	SCENETYPE			e_CurType = SCENE_LOGO;

	SNOWMAN_ATTACK		m_eAtkState = SNOWMANATK_END;

private:
	CTransform*			m_pTargetPlayer = nullptr;
	CCollider*			m_pMyCollider = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	// Dead
	//_float				m_fHp = 300.f;
	_uint				m_iPass = 1;
	_bool				m_bMinusHP = false;
	_float				m_fDeadTime = 0.f;

	_float				m_fAlpha = 0.f;


	_bool				m_bMotion = false;

	_bool				m_bFirst = false;

	_bool				m_bAttack = false;
	// JumpAttack
	_bool				m_bAttackBool = false;
	_bool				m_bJumpAttackCoolTime = true;
	_bool				m_bPattern0 = false;
	_bool				m_bAttack0 = false;

	_float				m_fAttackTime0 = 0.f;
	_float				m_fJumpAttackCoolTime = 0.f;
	_float				m_fJumpAttackStartTime = 0.f;
	

	// Jump
	_vec3				m_vJumpDir;
	_float				m_fJumpTime = 0.f;
	_float				m_fJumpPower = 17.f;
	_float				m_fPosY = 0.f;

	// Hit
	_float				m_fHitTime = 0.f;

	// Attack2 
	_bool				m_bAttackCoolTime2 = true;
	_bool				m_bPattern2 = false;

	_float				m_fAttackTime2 = 0.f;
	_bool				m_bSelCol = true;
	_float				m_fAttackStartTime = 0.f;
	_bool				m_bCheckAttack = true;

	// µÓ¿Â
	_bool				m_bOpenBoss = false;
	//_float				m_fAlpha = 0.f;
	_bool				m_bFullAlpha = false;

public:
	_bool		Get_View() { return m_bFirst; }
	void		Set_View(_bool	view) { m_bFirst = view; }
	void		Set_MinusHp() { m_fHp = -1.f; }
	void		Set_OpenView(_bool View) { m_bOpenBoss = View; }

private:
	void		Collision_ToPlayer();

public:
	static SnowmanBoss*	Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType);

private:
	virtual _ulong Free();
};

