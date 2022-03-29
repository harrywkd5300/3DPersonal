#pragma once

#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTerrain_Buffer;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CMaterial;
	class CShader;
}

class CBossTerrain final : public Engine::CGameObject
{
private:
	explicit CBossTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossTerrain() = default;
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CTerrain_Buffer*		m_pBufferCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CTexture*				m_pTextureTowerRangeCom = nullptr;
	Engine::CMaterial*				m_pMaterialCom = nullptr;
	Engine::CShader*				m_pShader = nullptr;
	Engine::CTransform*				m_pTargetBufferCom = nullptr;

	Engine::CTexture*				m_pDestCom0 = nullptr;
	Engine::CTexture*				m_pDestCom1 = nullptr;
	Engine::CTexture*				m_pDestCom2 = nullptr;
	Engine::CTexture*				m_pDestCom3 = nullptr;

private:
	LPDIRECT3DTEXTURE9				m_pFilterTexture = nullptr;

private:
	_bool							m_bTowerRadius = false;
	_vec3*							m_pVertexPos = nullptr;

public:
	void		Set_TowerRadius();
public:
	HRESULT Ready_Component(void);
	HRESULT	Ready_Height();
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
public:
	static CBossTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) final;

};


