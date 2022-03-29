#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMaterial;
	class CStatic_Mesh;
	class CCollider;
	class CFrustum;
	class CVIBuffer;
}


class CTestObj final : public CGameObject
{
private:
	explicit CTestObj(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestObj() = default; 

public:
	virtual HRESULT Ready_GameObject(const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CFrustum*			m_pFrustum = nullptr;

	CTransform*			m_pPlayerTrans = nullptr;

private:
	CVIBuffer*			m_pTargetBufferCom = nullptr;

	OBJECT*				m_pObjectData = nullptr;
	_bool				m_isDraw = true;

private:
	HRESULT		Ready_Component();
	HRESULT		Ready_MeshComponent(const _tchar* ComPath);
	void		Move_OnTerrain();

public:
	void		Collision_ToObject();

public:
	static CTestObj*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* FullPath, const _tchar*  FileName, const _tchar* ComPath, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot);

protected:
	virtual _ulong Free() final;

};

