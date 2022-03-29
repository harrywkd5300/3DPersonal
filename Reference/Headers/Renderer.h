#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

// 화면에 그려야할 객체들을 모아둔다.\
// 객체들을 그린다.
class CGameObject;
class DLL_EXPORT CRenderer final : public CComponent
{	
public:
	enum RENDER {RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END};
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer() = default;
public: // Setter
	HRESULT Add_RenderGroup(RENDER eRenderGroup, CGameObject* pGameObject);

public:
	HRESULT Ready_Renderer(void);
	void Render_RenderGroup(void);

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	typedef list<CGameObject*>	RENDERGROUP;
private:
	void Render_Priority(void);
	void Render_NonAlpha(void);
	void Render_Alpha(void);
	void Render_UI(void);
private:
	void Clear_RenderGroup(void);
public:	
	virtual CComponent* Clone(void) final;
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void);

};

END