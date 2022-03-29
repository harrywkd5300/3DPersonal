#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CStatic_Mesh;
	class CCollider;
	class CFrustum;
	class CMaterial;
	class CShader;
	//class CCollisionMgr;
}

class CBaseObj final : public CGameObject
{
private:
	explicit CBaseObj(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual	~CBaseObj() = default;

public:
	virtual HRESULT Ready_GameObject(const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot, SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	HRESULT				Ready_Component(void);
	HRESULT				Ready_MeshComponent(const _tchar* ComPath);

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CFrustum*			m_pFrustum = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

private:
	OBJECT*				m_pObjectData = nullptr;
	CTransform*			m_pPlayerTrans = nullptr;

private:
	_vec3				m_vRota;
	_bool				m_isDraw = true;
	_float				m_fFrustumSize = 2.f;

	_vec3				m_vPos = _vec3(0.f, 0.f, 0.f);

	SCENETYPE			m_eType;

	_bool				m_bAlphaStart = false;
	_float				m_fAlpha = 1.f;
private:
	void			Collision_ToObject();
	HRESULT			SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

public:
	_vec3*			Get_CurPos() { return &m_vPos; }
	CCollider*		Get_CollCom() { return m_pColliderCom; }
	OBJECT*			Get_ObjData() { return m_pObjectData; }


public:
	void			Set_AlphaStart() { m_bAlphaStart = true; }


public:
	static CBaseObj* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot, SCENETYPE eType);

private:
	virtual _ulong Free() final;
};

