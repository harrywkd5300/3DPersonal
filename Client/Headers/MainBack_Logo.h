#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRect_Texture;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CShader;
}

class CMainBack_Logo final : public Engine::CGameObject
{
private:
	explicit CMainBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMainBack_Logo() = default;
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;

private:
	_float			m_fX, m_fY;
	_float			m_fSizeX, m_fSizeY;

	_uint			iIdx = 0;
	_float			m_fTime = 0.f;

public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
public:
	static CMainBack_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) final;

};

