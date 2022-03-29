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
	//class CStatic_Mesh;
}

class CFrameEffect final : public Engine::CGameObject
{
private:
	explicit CFrameEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFrameEffect() = default;
public:
	virtual HRESULT Ready_GameObject(_vec3* vPos, _vec3* vRot, _vec3* vDir) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	//Engine::CStatic_Mesh*			m_pBufferCom = nullptr;

private:
	_uint							m_iTexIdx = 0;
	_float							m_fTimeDelta = 0.f;
	_float							m_fDegree = 0.f;

	_float							m_fRotaFirst = 0.f;

public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);

public:
	void	Set_Position(_vec3* vPos);

public:
	static CFrameEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vPos, _vec3* vRot, _vec3* vDir);
protected:
	virtual _ulong Free(void) final;

};

