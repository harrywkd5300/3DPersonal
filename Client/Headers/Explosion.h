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

class CExplosion final : public CGameObject
{
private:
	explicit CExplosion(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplosion() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, SCENETYPE eType);
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

	SCENETYPE			m_eType;

private:
	_float				m_fDeadTime = 0.f;
	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);

	_float				m_fAlpha = 0.9f;

	// UV Animation
	_float				m_fTimeDelta = 0.f;

	// Test
	_bool				m_bStart = false;
	

private:
	HRESULT Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Collision_ToPlayer();

public:
	static CExplosion*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, SCENETYPE eType);

protected:
	virtual _ulong Free();
};

