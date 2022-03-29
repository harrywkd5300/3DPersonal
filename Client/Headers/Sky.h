#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CCube_Texture;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CMaterial;
	class CShader;
}

class CSky final : public CGameObject
{
public:
	explicit CSky(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual  ~CSky() = default;

public:
	virtual HRESULT Ready_GameObject(SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CCube_Texture*			m_pBufferCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CMaterial*				m_pMaterialCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;

	SCENETYPE						m_eType;
public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
public:
	static CSky* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType);
protected:
	virtual _ulong Free(void) final;
};

