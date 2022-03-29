#pragma once

#include "Base.h"

#include "d3dfont.h"

BEGIN(Engine)

class DLL_EXPORT CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
public:
	enum WINMODE { MODE_FULL, MODE_WIN };
private:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	CD3DFont*	Get_Font() { return m_pD3dFont; }
	LPDIRECT3DDEVICE9	Get_GraphicDev() { return m_pGraphicDev; }
	LPD3DXLINE			Get_Line() { return m_pLine; }

public:
	HRESULT Ready_GraphicDev(LPDIRECT3DDEVICE9* ppGraphicDevPtr, HWND hWnd, WINMODE eMode, const _uint& iBackCX, const _uint& iBackCY);
private:
	LPDIRECT3D9					m_pSDK = nullptr;
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	LPD3DXFONT					m_pFont = nullptr;
	CD3DFont*					m_pD3dFont = nullptr;
	LPD3DXLINE				    m_pLine = nullptr;

private:
	virtual _ulong Free(void) final;
};

END