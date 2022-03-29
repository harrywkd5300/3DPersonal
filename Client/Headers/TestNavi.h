#pragma once

#include "gameObject.h"
#include "Defines.h"

namespace Engine
{
	class CRenderer;
	class CNavigation;
}


class CTestNavi final : public CGameObject
{
private:
	explicit CTestNavi(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestNavi() = default;

public:
	virtual HRESULT Ready_GameObject(SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	Engine::CNavigation*	m_pNavigationCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;

private:
	HRESULT		Ready_Component();

private:
	SCENETYPE	m_eType;
public:
	static CTestNavi*	Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENETYPE eType);

protected:
	virtual _ulong Free() final;

};

