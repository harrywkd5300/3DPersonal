#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamic_Mesh;
	class CMaterial;
	class CCollider;
	class CShader;
	class CNavigation;
}

#include "Wave_Manager.h"

class CMonster : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

public:
	HRESULT		Ready_Component();

protected:
	_float		m_fHp = 0.f;
	_float		m_fCollState = 1.f;

	_bool		m_bHit = false;
	_bool		m_bMinusHP = false;
	_bool		m_bSameMonster = false;
	SCENETYPE	m_eType = SCENE_STAGE;


	_bool		m_bCheckCol = false;
	_bool		m_bColMove = false;

	_float		m_fPlayerDist = 0.f;

	_float		m_fColTime = 0.f;
	CGameObject*		m_pShield = nullptr;

	/*private ComMember*/
protected:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CDynamic_Mesh*		m_pBufferCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;

public:
	// Setter
	void		Set_Hit(const _float& fAtk);
	void		Set_DeadShield() { m_pShield = nullptr; }
	void		Set_TrueCol(_bool	Col) { m_bSameMonster = Col; }
	void		Set_ColState(const _float& State) { m_fCollState *= State; }
	void		Set_CheckCol(_bool Col) { m_bCheckCol = Col; }
	void		Set_CheckColMove(_bool Col) { m_bColMove = Col; }

	// Getter
	_bool		Get_CheckCol() { return m_bCheckCol; }
	_bool		Get_CheckColMove() { return m_bColMove; }
	_float		Get_PlayerDist() { return m_fPlayerDist; }

protected:
	virtual _ulong Free();
};

