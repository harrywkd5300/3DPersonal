#pragma once

#include "scene.h"
#include "Defines.h"

class CScene_Boss final : public CScene
{
private:
	explicit CScene_Boss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Boss() = default;
public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;

private:
	HRESULT	Ready_LightInfo();
	HRESULT Add_BGMForStageBoss();
	HRESULT	Set_UI();
	HRESULT	Ready_Load_Object();
	HRESULT	Ready_Crystal(const _tchar * pLayerTag);
	HRESULT	Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT	Change_Camera();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Set_Player();
	HRESULT Set_Boss();
	HRESULT Ready_Layer_TEST(const _tchar* pLayerTag);

private:
	vector<OBJECT*>			m_vecObject;

public:
	static CScene_Boss* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) override;

};

