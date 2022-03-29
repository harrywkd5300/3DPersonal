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
	class CShader;
}

class CPlayer;
class CCrossBow : public CGameObject
{
private:
	explicit CCrossBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrossBow() = default;

public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

	_matrix				m_matWorld;

private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

public:
	_vec3*		Get_Position();

public:
	static CCrossBow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual _ulong Free() final;


};

