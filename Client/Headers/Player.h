#pragma once

#include "Defines.h"
#include "GameObject.h"


namespace Engine
{
	class CRenderer;
	class CTransform;
	class CPicking;
	class CVIBuffer;
	class CInput_Device;
	class CDynamic_Mesh;
	class CMaterial;
	class CCollider;
	class CShader;
	class CNavigation;
}

class CPlayer final : public CGameObject
{
	enum PLAYER_STATE { STATE_STAND, STATE_FORWARD, STATE_RIGHT, STATE_LEFT  , STATE_BACKWORD, STATE_ATTACK, STATE_JUMP,   STATE_JUMPFALL,
						STATE_SPAWN, STATE_WIN    , STATE_TURN , STATE_DEATH , STATE_HURT    , STATE_RELOAD, STATE_SUMMON, STATE_REPAIR,
						STATE_LOWIDLE, STATE_END };
public:
	enum HIT_STATE { SMALL_HIT, SMASH_HIT, END_HIT  };
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

	/*private ComMember*/
private:
	CTransform*			m_pTransformCom = nullptr;
	CPicking*			m_pPickingCom = nullptr;
	CRenderer*			m_pRendererCom  = nullptr;
	CVIBuffer*			m_pTargetBufferCom = nullptr;
	CDynamic_Mesh*		m_pBufferCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;

	CInput_Device*		m_pInput_Device = nullptr;

	/*private Member*/
private:
	CGameObject*		m_pBow = nullptr;
	D3DXMATRIX*			m_pRHandMatrix;

	_float				m_fTimeDelta = 0.f;
	_float				m_fAttackTime = 0.f;
	_float				m_fAttackCount = 10.f;
	_float				m_fAttackMaxCount = 10.f;
	_float				m_fMouseMoveSpeed = 7.f;


	_uint				m_iCheckIdx = -1;
	// AniTime
	_float				m_fAniTime = 1.f;
	_double				m_fDelayTime = 0.f;

	// Jump
	_vec3				m_vJumpNextPos;
	_vec3				m_vJumpDir;
	_float				m_fJumpFall = 0.f;
	_float				m_fJumpTime = 0.f;
	_float				m_fJumpPower = 7.f;
	_float				m_fPosY = 0.f;

	_bool				m_bJump = false;
	_bool				m_bJumpFall = false;
	_bool				m_bJumpMove = false;

	// HIt
	_bool				m_bHit = false;
	_bool				m_bHitCol = false;
	_float				m_fHitColTime = 0.f;

	_bool				m_bFailBuild = true;

	PLAYER_STATE		m_eCurrState = STATE_STAND;
	PLAYER_STATE		m_eNewState  = STATE_END;

	HIT_STATE			m_eCurHit = END_HIT;

	_bool				m_bMotion = false;
	_bool				m_bTurnRight = false;
	_bool				m_bTurnLeft = false;
	_bool				m_bSummon = false;
	_bool				m_bLowHp = false;
	_bool				m_bAttack = true;
	_bool				m_bNotWeapon = false;
	_bool				m_bReload = false;
	_bool				m_bMoveJump = false;
	_bool				m_bNotUpdate = false;
	_bool				m_bColl = false;
	_bool				m_bStaticCamera = true;


	// Double Key Input Check
	_bool				KeyCheck6 = true;
	_float				m_fCheck6 = 0.f;
	_bool				m_bBuild = false;

	_bool				m_bBuildLighting = true;
	_bool				m_bBuildStrike = true;
	_bool				m_bBuildMisile = true;
	_bool				m_bBuildShilder = true;

	SCENETYPE			e_CurType = SCENE_LOGO;

	// Test
	_bool				m_bKeyTest = true;
	_float				m_fKeyTest = 0.f;

	// Sound
	_float				m_fFootWalk = 0.f;
	_bool				m_bFootWalk = true;

	/*private Funtion*/
private:
	HRESULT		Ready_Component();
	void		Move_OnTerrain();
	void		Key_Input(const _float & fTimeDelta);
	void		State_Update();
	void		Jump(const _float& fTimeDelta);
	void		Attack(const _float& fTimeDelta);
	void		Hit(const _float& fTimeDelta);
	void		CollTime(const _float& fTimeDelta);
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

	void		Collision_ToObject();


	/* public Function */
public:
	void		Set_EndBuild() { m_bBuild = true; }
	void		Set_UpdateBool(_bool Update) { m_bNotUpdate = Update; }


	CTransform*	Get_Transform() { return m_pTransformCom; }

	/*public Function*/
	void		Set_CurType(SCENETYPE eTpye) { e_CurType = eTpye; }
	void		Set_IsColl(_bool Coll);
	void		Release_Component();
 
public:
	CGameObject*	Get_Weapon() { return m_pBow; }
	void			Set_Position(_vec3* vPos);
	void			Set_Scale(_vec3* vPos);
	void			Set_Coll(_bool	isColl) { m_bColl = isColl; }
	void			Set_Hit(_bool	isHit) { m_bHit = isHit; }
	void			Set_PlayerHitState(HIT_STATE eState) { m_eCurHit = eState; }

public:
	static CPlayer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free() final;
};

