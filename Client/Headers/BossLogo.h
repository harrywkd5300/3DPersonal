#pragma once

#include "Defines.h"
#include "Scene.h"

namespace Engine
{
	class CD3DFont;
}
class CLoading;
class CBossLogo : public Engine::CScene
{
private:
	explicit CBossLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossLogo() = default;

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;

private:
	CLoading*			m_pLoading = nullptr;
	CD3DFont*			m_pD3dFont = nullptr;

	_tchar				m_szThread[128] = L"";

private:
	HRESULT Ready_LogoComponent(void);
	HRESULT	Ready_D3dFont();
	HRESULT	Change_Camera();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT	Set_UI();

private:
	HRESULT Change_Scene(SCENETYPE eSceneID);
public:
	static CBossLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void) override;

};

