#pragma once

#include "GameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CCollider;
	class CShader;
	class CMaterial;

	class CRect_Texture;
	class CTexture;
}

class CFire_Explosion final : public CGameObject
{
private:
	explicit CFire_Explosion(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFire_Explosion() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3 * vScale, const _uint& iIdx, SCENETYPE eType);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CRect_Texture*		m_pBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CTransform*			m_pPlayerTrans = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;

	_uint				m_iIdx = 0;
	_uint				m_iMaxIdx = 0;

	SCENETYPE eType;

private:
	_float				m_fDeadTime = 0.f;

	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vEndPos = _vec3(0.f, 0.f, 0.f);

	_vec3				m_vLook = _vec3(0.f, 0.f, 0.f);
	_vec3				m_vScale = _vec3(0.f, 0.f, 0.f);

	_float				m_fLerp = 0.f;

	// Test
	_bool				Start = false;

	// UV Animation
	_float				m_fTimeDelta = 0.f;

	_float				m_fAniFrame = 0.f;

private:
	HRESULT Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void	Collision_ToPlayer();

public:
	static CFire_Explosion*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3 * vScale, const _uint& iIdx, SCENETYPE eType);

protected:
	virtual _ulong Free();
};

