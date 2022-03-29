#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamic_Mesh;
	class CCollider;
	class CMaterial;
	class CShader;
}

class CShield final : public CGameObject
{
private:
	explicit CShield(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShield() = default;

public:
	virtual HRESULT Ready_GameObject(_vec3* vPos, SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CDynamic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;

	CTransform*			m_pPlayerTrans = nullptr;

	SCENETYPE			m_eType = SCENE_END;

private:
	_float				m_fTimeDelta = 0.f;
	_bool				m_bNotBuild = true;

	_float				m_fHp = 30000.f;

	_float				m_fAlpha = 1.f;
	_uint				m_iPass = 1;

	list<CGameObject*>	m_Monster;

private:
	HRESULT		Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Not_BuildMove();
	void		Collision_ToPlayer();
	void		Dead_To_MonsterTalk();

public:
	void		Set_CompleteBuild(_bool Complete) { m_bNotBuild = Complete; }
	_bool		Get_NotBuild() { return m_bNotBuild; }
	void		Set_Dgree(_vec3* vDgree);
	void		Set_ListObj(CGameObject* pObject);

	CCollider*	Get_Collider() { return m_pColliderCom; }

public:
	static CShield*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, SCENETYPE eType);

protected:
	virtual _ulong Free() final;

};