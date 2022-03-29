#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMaterial;
	class CShader;
	class CBossPaticle;
}


class CBoss_Paticle final : public CGameObject
{
private:
	explicit CBoss_Paticle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss_Paticle() = default;

public:
	virtual HRESULT Ready_GameObject(CTransform* Target) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CBossPaticle*		m_pBossPaticleCom = nullptr;

	CTransform*			m_pTargetTrans = nullptr;

private:
	_vec3				m_vCrystal = _vec3(0.f, 0.f, 0.f);
	_bool				m_bCrystalGo = false;
	_bool				m_bCrystalGo2 = false;

private:
	HRESULT		Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Set_BossPaticle();

public:
	void		Set_TargetTrans(CTransform* m_pTarget) { m_pTargetTrans = m_pTarget; }
	void		Set_DeadPaticle();
public:
	void		Set_Crystal(_vec3* vCrystal) { m_vCrystal = *vCrystal; }
	void		Set_BoolCrystal(_bool Crystal) { m_bCrystalGo = Crystal; }
	void		Set_BoolCrystal2(_bool Crystal) { m_bCrystalGo2 = Crystal; }

public:
	static CBoss_Paticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* Target);

private:
	virtual _ulong Free() final;
};

