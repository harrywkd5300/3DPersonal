#pragma once

#include "gameObject.h"
#include "Tool_Define.h"

namespace Engine
{
	class CRenderer;
	class CCube_Texture;
	class CTransform;
}

class CPickObj : public CGameObject
{
private:
	explicit CPickObj(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual ~CPickObj() = default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	HRESULT				Ready_Component(void);

private:
	CCube_Texture*		m_pBufferCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	_bool				m_pClick = false;

public:
	void				Set_Position(const _vec3& vPos);

public:
	static CPickObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free() final;

};

