#pragma once

#include "Gameobject.h"
#include "Tool_Define.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CTerrain_Buffer;
	class CTexture;
	class CShader;
	class CMaterial;
	class CPicking;
}

class CToolTerrain final : public CGameObject
{
private:
	explicit CToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual	~CToolTerrain()= default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	HRESULT				Ready_Component(void);
	HRESULT				Ready_Filter();

public:
	HRESULT				Ready_TextureCom(const _uint& iIdx, const _tchar* ComName);
	HRESULT				Alpha_Texture_Init();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom	= nullptr;
	CTerrain_Buffer*	m_pBufferCom	= nullptr;
	CPicking*			m_pPickingCom   = nullptr;

	CTexture*			m_pTexture		= nullptr;
	CTexture*			m_pTextureBrush = nullptr;
	CTexture*			m_pDestCom0 = nullptr;
	CTexture*			m_pDestCom1 = nullptr;
	CTexture*			m_pDestCom2 = nullptr;
	CTexture*			m_pDestCom3 = nullptr;

	CShader*			m_pShader		= nullptr;
	CMaterial*			m_pMaterialCom  = nullptr;

private:
	LPDIRECT3DTEXTURE9	m_pFilterTexture = nullptr;

private:
	_uint				m_iTextureNum = 1;
	_bool				m_bViewWireFrame = false;
	_bool				m_bViewBrush = true;
	_float				m_fBrushSize = 5.f;

	POINT				m_pMousePos;

	_float				m_fTimeDelta = 0.f;

public:
	void				Change_TerrianBuffer();
	void				Set_TextureNum(const _uint& iTexNum) { m_iTextureNum = iTexNum; }
	void				Set_WireFrame(_bool	View) { m_bViewWireFrame = View; }
	HRESULT				SetUp_OnContantTable(LPD3DXEFFECT pEffect);

	void				Set_ViewBrush(_bool	View) { m_bViewBrush = View; }
	void				Set_BrushSize(const _float& fSize) { m_fBrushSize = fSize; }
	void				Set_MousePoint(POINT pos) { m_pMousePos = pos; }
	void				Set_Spletting(const _uint & iIdx);
	void				Make_Height();


	void				Plus_Height(const _float& fSize, const _float& fMaxSize);
	
	LPDIRECT3DTEXTURE9	Get_FilterTextrue() { return m_pFilterTexture; }

	_vec3*				Get_TerrainBufferData();
	const _vec3*				Get_TerrainData();

	void				Load_TerrainData(const _vec3* vPosition);

	/*related ctor dtor*/
public:
	static CToolTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free() final;

};

