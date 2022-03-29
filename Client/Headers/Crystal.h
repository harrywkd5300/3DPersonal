#pragma once

#include "GameObject.h"
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

class CCrystal final : public CGameObject
{
private:
	explicit CCrystal(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrystal() = default;

public:
	virtual HRESULT Ready_GameObject();
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
	_vec3				m_vStartPos = _vec3(0.f, 0.f, 0.f);
	_float				m_A = 1.f;
public:
	static CCrystal* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free() final;

};

