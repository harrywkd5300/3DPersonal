#pragma once

#include "gameObject.h"
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
class CDragonArrow final : public CGameObject
{
private:
	explicit CDragonArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDragonArrow() = default;


public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3* vDir, _vec3* vRot, CGameObject* pObject, SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;

	CWave_Manager*		m_pWave_Manager = nullptr;
	SCENETYPE			m_eType = SCENE_END;

private:
	CGameObject*		m_pDragonKiller = nullptr;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fTimeShader = 0.f;

	CCollider*			m_pTargetCol = nullptr;
	CGameObject*		m_pMonster = nullptr;
	//CGameObject*		m_pTower = nullptr;

	_vec3				m_vDir = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);
	_float				m_fDist = 0.f;
private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void		Collision_ToTarget();

public:
	void	Set_CollBox(CCollider* pCollider) { m_pTargetCol = pCollider; }
	void	Set_LookMonster(CGameObject* pObject) { m_pMonster = pObject; }

public:
	static CDragonArrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3* vDir, _vec3* vRot, CGameObject* pObject, SCENETYPE eType);

protected:
	virtual _ulong Free() final;

};

