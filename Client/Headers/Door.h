#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CStatic_Mesh;
	class CMaterial;
	class CCollider;
	class CShader;
	class CNavigation;
}

class CDoor final : public CGameObject
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Ready_GameObject(_bool Left, _vec3* vPos, _vec3* vRot);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

	/*private ComMember*/
private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

private:
	_bool				m_bLeft = true;
	_vec3				m_vRota = _vec3(0.f, 0.f, 0.f);
	_bool				m_bOpen = false;
	_vec3				m_vOpen = _vec3(0.f, 0.f, 0.f);

private:
	void				Open_Door(const _float& fTimeDelta);
	void				Collision_ToPlayer();

public:
	void				Set_OpenDoor() { m_bOpen = true; }
	void				Set_OpenDegree(const _vec3* vRot) { m_vOpen = *vRot; }

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool Left, _vec3* vPos, _vec3* vRot);

private:
	virtual _ulong Free() final;

};

