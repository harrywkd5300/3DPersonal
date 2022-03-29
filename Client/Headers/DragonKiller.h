#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamic_Mesh;
	class CCollider;
	class CMaterial;
	class CShader;
}
class CWave_Manager;
class CDragonKiller final : public CGameObject
{
	enum DRAGON_STATE { DRAGON_STAND, DRAGON_ATTAKC, DRAGON_END };
private:
	explicit CDragonKiller(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDragonKiller() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vPos, SCENETYPE eType) final;
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

	CWave_Manager*		m_pWave_Manager = nullptr;

	CTransform*			m_pPlayerTrans = nullptr;
	SCENETYPE			m_eType = SCENE_END;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

private:
	DRAGON_STATE		m_eCurrState = DRAGON_STAND;
	DRAGON_STATE		m_eNewState = DRAGON_END;

private:
	CTransform*			m_pTargetMonster = nullptr;
	CGameObject*		m_pArrow = nullptr;
	CGameObject*		m_pMonster = nullptr;


	_bool				m_bNotBuild = true;


	_float				m_fAlpha = 1.f;
	_uint				m_iPass = 1;

	_bool				m_bMotion = false;
	_bool				m_bFirst = false;

	_bool				m_bAnimation = false;

	// Attack
	_bool				m_bMakeAttack = false;
	_float				m_fAttackTime = 0.f;

	_bool				m_bAttack = false;

	_bool				m_bAttackCoolTime = false;
	_float				m_fAttackCoolTime = 0.f;
	_bool				m_bMakeArrow = false;
	_float				m_fMakeArrow = 0.f;

	_bool				m_bStartAttack = false;
	_float				m_fStartTime = 1.1f;
	_bool				m_aniFirst = true;

	_vec3				m_vTargetPos = _vec3(0.f, 0.f, 0.f);
	_bool				m_bAttackBool = true;
	_bool				m_bAttacking = false;	
	_bool				m_bAttackStart = false;

	_float				m_fAttackGo = 0.f;
private:
	HRESULT		Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Not_BuildMove();
	void		Collision_ToPlayer();

	void	State_Update();
	void	Move_Partern(const _float& fTimeDelta);
	void	Attack(const _float& fTimeDelta);
	void	CollTime(const _float& fTimeDelta);

public:
	void		Set_CompleteBuild(_bool Complete) { m_bNotBuild = Complete; }
	_bool		Get_NotBuild() { return m_bNotBuild; }
	void		Set_Dgree(_vec3 * vDgree);

	void		Set_DeadWeapon() { m_pArrow = nullptr; }
public:
	static CDragonKiller*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eType);

protected:
	virtual _ulong Free() final;

};