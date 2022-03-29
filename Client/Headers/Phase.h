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

class CPhase final : public Engine::CGameObject
{
private:
	explicit CPhase(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPhase() = default;
public:
	virtual HRESULT Ready_GameObject(const _float& fTexNum, SCENETYPE eType) final;
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
	_float			m_fTextureNum = 0;
	_float			m_fAlpha = 1.f;
	_float			m_fCnt = 0.f;
	_float			m_fMoveTime = 0.f;

	SCENETYPE		m_eType;

public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
public:
	static CPhase* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fTexNum, SCENETYPE eType);
protected:
	virtual _ulong Free(void) final;

};

