#pragma once


#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamic_Mesh;
	class CNavigation;
	class CCollider;
	class CMaterial;
	class CShader;
}


class CSanta final : public CGameObject
{
	enum SANTA_STATE { SANTA_IDLE, SANTA_FORWARD, SANTA_DEATH, SANTA_CALLOUT, SANTA_TURNLEFT,
					   SANTA_HURT, SANTA_DROPITEM, SANTA_WINWAVE, SANTA_SPAWN, SANTA_END };

private:
	explicit CSanta(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSanta() = default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CDynamic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CNavigation*		m_pNavigation = nullptr;

	CTransform*			m_pPlayerTrans = nullptr;

private:
	_bool				m_bBattleTime = true;

private:
	SANTA_STATE			m_eCurrState = SANTA_IDLE;
	SANTA_STATE			m_eNewState = SANTA_END;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_bool				m_bMotion = false;

	_float				m_fAlpha = 1.f;
	_uint				m_iPass = 1;

	// Move
	_float				m_fMoveTime = 0.f;
	_bool				m_bMoveBool = true;
	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vDir = _vec3(0.f, 0.f, 0.f);
	_bool				GoXY = true;
	_bool				GoYZ = true;
	_bool				GoXZ = true;

	// DropItem
	_bool				m_bDropItem = false;
	_bool				m_bPutItem = false;
	_bool				m_bEndItem = true;

	_float				m_fDropItem = 0.f;
	_float				m_fPutTime = 0.f;

	// Telepoter
	_bool				m_pTelepoter = false;

	_float				m_fTelepoter = 0.f;

	_float				m_fSuper = 0.f;

private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

	void		State_Update();
	void		Move_Partern(const _float& fTimeDelta);
	void		CollTime(const _float& fTimeDelta);

	void		Collision_ToObject();

public:
	void		Set_BattleTime(_bool Time) { m_bBattleTime = Time; }
	void		Set_Super() { ++m_fSuper; };
	void		Set_SuperMax() { m_fSuper += 6.f; };

public:
	static CSanta*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual _ulong Free() final;

};

