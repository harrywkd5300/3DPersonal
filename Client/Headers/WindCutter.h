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

class WindCutter final : public CGameObject
{
private:
	explicit WindCutter(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~WindCutter() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3* vLook, _vec3* vEndPos, SCENETYPE eType);
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


	CTransform*			m_pPlayerTrans = nullptr;

	SCENETYPE			m_eType = SCENE_END;

	// Testing
	_bool				m_bStart = false;

	_float				m_fAlpha = 1.f;

	// UV Animation
	_float				m_fTimeDelta = 0.f;

private:
	_float				m_fDeadTime = 0.f;

	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);

	_vec3				m_vLook = _vec3(0.f, 0.f, 0.f);

	_vec3               m_ColDgree = _vec3(0.f, 0.f, 0.f);


private:
	HRESULT Ready_Component();
	HRESULT Ready_Collider();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Collision_ToPlayer();

public:
	static WindCutter*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3* vLook, _vec3* vEndPos, SCENETYPE eType);

protected:
	virtual _ulong Free();
};

