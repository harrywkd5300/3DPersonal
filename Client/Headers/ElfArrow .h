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


class CElfArrow final : public CGameObject
{
private:
	explicit CElfArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CElfArrow() = default;


public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3* vDir, _vec3* vRot) final;
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

	_vec3				m_vDir;
	_float				m_fDist;

	CTransform*			m_pTargetPlayer = nullptr;

	CGameObject*		m_pShield = nullptr;
	CGameObject*		m_pMonster = nullptr;

private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

	void		Coll_ToPlayer();

public:
	void		Set_TargetShield(CGameObject* pObject) { m_pShield = pObject; }
	void		Set_TargetMonster(CGameObject* pObject) { m_pMonster = pObject; }

public:
	static CElfArrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3* vDir, _vec3* vRot);

protected:
	virtual _ulong Free() final;

};

