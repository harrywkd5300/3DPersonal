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


class CItem final : public CGameObject
{
private:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem() = default;


public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3* vLook) final;
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

private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

	void		Collision_ToObject();

public:
	static CItem*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3* vLook);

protected:
	virtual _ulong Free() final;

};

