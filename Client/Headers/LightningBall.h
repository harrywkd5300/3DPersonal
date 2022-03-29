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
class CLighningBall final : public CGameObject
{
private:
	explicit CLighningBall(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLighningBall() = default;

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

	CCollider*			m_pTargetCol = nullptr;
	CGameObject*		m_pMonster = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;
	SCENETYPE			m_eType = SCENE_END;

private:
	_float				m_fTimeDelta = 0.f;
	_float				m_fTimeShader = 0.f;

	_bool				m_bEndMake = true;

	_vec3				m_vFirstScale = _vec3(0.00001f, 0.00001f, 0.00001f);

	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vDir = _vec3(0.f, 0.f, 0.f);

	_bool				m_bCheck = true;
	_bool				m_bAttakMove = false;

	_float				m_fDist = 0.f;

	_float				m_fY = 0.f;

	_bool				m_bBigMake = true;

	_float				m_fDeadTime = 0.f;
private:
	CGameObject*		m_pTower = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void	Collision_ToTarget();

public:
	void	Set_EndPos(_vec3* vEnd) { m_vEndPos = *vEnd; }
	void	Set_CollBox(CCollider* pCollider) { m_pTargetCol = pCollider; }
	void	Set_LookMonster(CGameObject* pObject) { m_pMonster = pObject; }

public:
	static CLighningBall*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vMakePos, CGameObject* pObject, SCENETYPE eType);

protected:
	virtual _ulong Free();
};

