#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMaterial;
	class CShader;
	class CSnowPaticle;
}


class CBoss_Paticle final : public CGameObject
{
private:
	explicit CBoss_Paticle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss_Paticle() = default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CSnowPaticle*		m_pSnowPaticleCom = nullptr;

	CTransform*			m_pTargetTrans = nullptr;
private:
	HRESULT		Ready_Component();
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

private:
	void		Set_SnowBox();

public:
	static CBoss_Paticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free() final;
};

