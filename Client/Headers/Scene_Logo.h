#pragma once

#include "Defines.h"
#include "Scene.h"

namespace Engine
{
	class CD3DFont;
}

class CLoading;
class CScene_Logo : public Engine::CScene
{
private:
	explicit CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Logo() = default;
public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
	HRESULT					Add_Logo_BGM();
private:
	CLoading*			m_pLoading = nullptr;
	CD3DFont*			m_pD3dFont = nullptr;

	_tchar				m_szThread[128] = L"";
private:
	HRESULT Ready_LogoComponent(void);
	HRESULT	Ready_D3dFont();
	HRESULT Ready_Layer_Camera(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

private:
	HRESULT Change_Scene(SCENETYPE eSceneID);
public:
	static CScene_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) override;
};

