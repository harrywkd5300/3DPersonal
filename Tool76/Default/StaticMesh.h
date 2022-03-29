#pragma once

#include "gameObject.h"
#include "Tool_Define.h"


namespace Engine
{
	class CRenderer;
	class CTransform;
	class CStatic_Mesh;
	class CCollider;
	class CFrustum;
	class CMaterial;
}

class CStaticMesh : public CGameObject
{
private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual	~CStaticMesh() = default;

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

	OBJECT*				m_pObjectData = nullptr;

private:
	_bool				m_bViewCollBox = true;
	_bool				m_isDraw = true;

	_bool				m_bViewWire = false;

	_vec3				m_vRota;

public:
	LPD3DXMESH*			Get_Mesh();
	void	Get_WorldMatrix(_matrix* matWorld);
	_bool	Get_RedCollBox();
	//CComponent*		Get_Transform() { return m_pTransformCom; }

	void	Set_Wire(_bool Wire) { m_bViewWire = Wire; }


	OBJECT*&	Get_ObjectData();

	void	Set_ColliderBox(_bool View) { m_bViewCollBox = View; }
	void    Set_PickObjectCollider(_bool Coll);
	
public:
	static CStaticMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot);

private:
	virtual _ulong Free() final;

};

