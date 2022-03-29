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
}

class CTree final : public CGameObject
{
private:
	explicit CTree(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual	~CTree() = default;

public:
	virtual HRESULT Ready_GameObject(const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot) final;
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

private:
	_vec3				m_vRota;
	_bool				m_isDraw = true;
	_bool				m_bNotDraw = false;

private:
	HRESULT				SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	void				Set_Draw(_bool	Draw) { m_bNotDraw = Draw; }

public:
	static CTree* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot);

private:
	virtual _ulong Free() final;
};

