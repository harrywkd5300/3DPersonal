#pragma once

#include "GameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CStatic_Mesh;
	class CCollider;
	class CShader;
	class CMaterial;
}

class CWave_Manager;
class CTowerMisile final : public CGameObject
{
private:
	explicit CTowerMisile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTowerMisile() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vMakePos, CGameObject* pObject, SCENETYPE eType);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;

	CWave_Manager*		m_pWave_Manager = nullptr;

	CCollider*			m_pTargetCol = nullptr;
	CGameObject*		m_pMonster = nullptr;
	SCENETYPE			m_eType = SCENE_END;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fTimeShader = 0.f;

	_float				m_fDist = 0.f;
private:
	CGameObject*		m_pTower = nullptr;
	_vec3				m_vDir = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_bool				m_bAttack = false;

private:
	HRESULT Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void	Collision_ToTarget();
public:
	void	Set_TowerAttack(_vec3* vMakePos, _vec3* vDir);
	void	Set_CollBox(CCollider* pCollider) { m_pTargetCol = pCollider; }
	void	Set_LookMonster(CGameObject* pObject) { m_pMonster = pObject; }

public:
	static CTowerMisile*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vMakePos, CGameObject* pObject, SCENETYPE eType);

protected:
	virtual _ulong Free();
};

