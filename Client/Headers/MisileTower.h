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
class CMisileTower final : public CGameObject
{
	enum MISILE_STATE { MISILE_STAND, MISILE_ATTAKC, MISILE_END };
private:
	explicit CMisileTower(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMisileTower() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vPos, SCENETYPE eTpye) final;
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
	CGameObject*		m_pWeapon = nullptr;

private:
	D3DXMATRIX*			m_pHandMatrix = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fTimeShader = 0.f;

	_bool				m_bNotBuild = true;
	_bool				m_bMotion = false;
	_float				m_fDelayTime = 0.f;
	_float				m_fAniTime = 1.f;

	_float				m_fAlpha = 1.f;
	_uint				m_iPass = 1;

	// 처음 웨폰 만들기
	_bool				m_bMakeWeapon = true;

	_vec3				m_vTargetMont = _vec3(0.f, 0.f, 0.f);

	// 공격 쿨타임
	_float				m_fAttakTime = 0.f;
	_bool				m_bAttackBool = true;

	// 공격 시작
	_bool				m_bAttackStart = false;
	_bool				m_bAttacking = false;

	// 공격중 미사일 발사 시간
	_float				m_fAttackGo = 0.f;

	// 미사일 재생성 시간
	_bool				m_bReLoadTime = false;
	_float				m_fReLoadTime = 0.f;

	_bool				m_bAttackPlay = true;


private:
	MISILE_STATE		m_eCurrState = MISILE_STAND;
	MISILE_STATE		m_eNewState  = MISILE_END;

private:
	CTransform*			m_pTargetMonster = nullptr;

private:
	HRESULT		Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Not_BuildMove();
	void		Collision_ToPlayer();
	void		State_Update();
	void		Move_Partern(const _float& fTimeDelta);
	void		Attack(const _float& fTimeDelta);
	void		CollTime(const _float& fTimeDelta);

public:
	void		Set_CompleteBuild(_bool Complete) { m_bNotBuild = Complete; }
	_bool		Get_NotBuild() { return m_bNotBuild; }
	void		Set_DeadWeapon() { m_pWeapon = nullptr; }

public:
	static CMisileTower*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eTpye);

protected:
	virtual _ulong Free() final;

};