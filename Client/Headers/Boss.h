#pragma once

#include "Monster.h"
#include "Defines.h"

namespace Engine
{
	class CBossPaticle;
}

class CBoss final : public CMonster
{
	enum BOSS_STATE {
		STATE_STAND, STATE_FORWARD, STATE_TURN, STATE_DEATH, STATE_HURT
		, STATE_ATTACK, STATE_ATTACK1, STATE_ATTACK2, STATE_BIGATTACK
		, STATE_BIGATTACK2, STATE_MAKE, STATE_END
	};

	enum SKILL_STATE { SKILL_EXPLOSION, SKILL_MISSILE,
					   SKILL_CUTTER, SKILL_END };

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Ready_GameObject(SCENETYPE eType);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	HRESULT Ready_Mesh();
	HRESULT Ready_Collider();
	void	Make_Snowman();
	void	Go_Church();
	void	State_Update();
	void	Move_Partern(const _float& fTimeDelta);
	void	Attack(const _float& fTimeDelta);
	void	CollTime(const _float& fTimeDelta);
	void	Using_Skill(const _float& fTimeDelta);


public:
	void	Release_Component();

	void	Set_Position(_vec3 * vPos);
	void	Set_Scale(_vec3 * vPos);

private:
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	BOSS_STATE		m_eCurrState = STATE_STAND;
	BOSS_STATE		m_eNewState  = STATE_END;

	SKILL_STATE		e_CurSkill = SKILL_END;

private:
	CTransform*			m_pTargetPlayer = nullptr;
	CCollider*			m_pRightCol = nullptr;
	CCollider*			m_pLeftCol = nullptr;

	D3DXMATRIX*			m_pRHandMatrix;
	D3DXMATRIX*			m_pLHandMatrix;

	CGameObject*		m_pPaticle = nullptr;


private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;

	_bool				m_bMotion = false;
	_bool				m_bAttackMotion = false;
	// State
	_float				m_fHp = 300.f;
	_float				m_fAniTime = 1.f;

	_float				m_fStagePt = 0.f;
	_bool				m_bStagePt = false;
	// BaseAttack
	_float				m_fBaseAttack = 0.f;
	_bool				m_bBaseAttack = true;
	_float				m_fAttackTime = 0.f;

	// GoAttack
	_float				m_fAttack1 = 0.f;
	_bool				m_bAttack1 = true;
	_float				m_fAttackTime1 = 0.f;
	_bool				m_bPattern0 = false;

	// Using Skill
	_bool				m_bUsingSkill = false;

	// Explosion
	_bool				m_bPattern1 = false;
	_bool				m_bAttack2 = false;
	_bool				m_bSkillCoolTime1 = true;

	_float				m_fSkillCoolTime1 = 0.f;
	_float				m_fSkillIdleTime = 0.f;

	// Explosion Bomb
	_bool				m_bSkillCoolTime2 = true;
	_bool				m_bPattern2 = false;
	_bool				m_bAttack3 = false;

	_float				m_fSkillCoolTime2 = 0.f;
	_float				m_fSkillMakeTime = 0.f;
	_float				m_fSkillCnt = 0.f;
	_float				m_fSkillPos = 1.f;
	_float				m_fSkillStartTime = 0.f;

	_float				m_fEndSkill0 = 0.f;
	// WindCutter
	_bool				m_bSkillCoolTime3 = true; //
	_bool				m_bPattern3 = false;
	_bool				m_bAttack4 = false; //

	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);
	
	_float				m_fSkillCoolTime3 = 0.f;
	_float				m_fSkillTime3 = 0.f;

	_bool				m_bSkillEndTime = true;
	_float				m_fSkillEndTime4 = 0.f;

	// Dead
	_bool				m_bMinusHP = false;
	_float				m_fAlpha = 1.f;
	_int				m_iPass = 1;

	_float				m_fDeadTime = 0.f;

	// For.MakeSonwManStage
	_bool				m_bFirst = false;
	_bool				m_bMakeSnowMan = false;
	_bool				m_bMakeEnd = false;

	_bool				m_pStartPos = true;
	_bool				m_bStartY = false;
	_float				m_fStartY = 0.f;
	_bool				m_bMakeMotion = false;
	_float				m_fMakeMotion = 0.f;
	_bool				m_bAlphaStart = true;

	// Boss Go
	_bool				m_bGoBossStage = false;
	_bool				m_bMakeBossEnd = false;
	_bool				m_pStartBossPos = true;
	_bool				m_bStartZ = false;
	_float				m_fStartZ = 0.f;

	// Bulid Page
	_bool				m_bBuildPage = true;
	_float				m_fEndTime = 0.f;
public:
	// Setter
	void				Set_BuildPage(_bool m_bBuild) { m_bBuildPage = m_bBuild; }

	void				Set_HpMinus() { m_fHp -= 110.f; }
	void				Set_SkillEnd() { m_bSkillEndTime = false; }
	void				Set_MakeSnowMan(_bool View) {  m_bFirst = View; }
	void				Set_StartSnowMan(_bool Start) { m_bMakeSnowMan = Start; }
	void				Set_StartBoss(_bool Start) { m_bGoBossStage = Start; }
	void				Set_CurType(SCENETYPE eTpye) { m_eType = eTpye; }

	// Getter
	_bool				Get_BuildPage() { return m_bBuildPage; }

public:
	static CBoss*	Create(LPDIRECT3DDEVICE9 pGraphicDev,  SCENETYPE eType);

private:
	virtual _ulong Free();

};

