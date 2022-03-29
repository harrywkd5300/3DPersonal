#pragma once

#include "Scene.h"
#include "Tool_Define.h"

#include "StaticMesh.h"
#include "DynamicObject.h"

namespace Engine
{
	class CD3DFont;
	class CCell;
}

class CToolScene : public CScene
{
private:
	explicit CToolScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CToolScene() = default;

public:
	virtual HRESULT		Ready_Scene(void) override;
	virtual _int		Update_Scene(const _float& fTimeDelta) override;
	virtual void		Render_Scene(void) override;
public:
	HRESULT		Ready_D3dFont();
	HRESULT		Ready_LightInfo();

	HRESULT		Add_MsehComponent(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComName, bool isStatic);
	HRESULT		Add_MeshObject(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComName, const _vec3& vMakeScale, const _vec3& vMakePos, const _vec3& vMakeRot, const _bool& vStatic);
	HRESULT		Add_Terrain(const _uint& iVtxX, const _uint& iVtxZ, const _float& iInterval = 1.f);
	HRESULT		Change_Texture_view(_bool	ViewWireFrame);

public:
	void		Terrain_Picking(_vec3* vPickPos, _ulong uX, _ulong uY);
	_bool		Make_Navigation(_vec3* vPickPos);
	void		Reserve_Navigation(const _ulong& Size);
	vector<CCell*>*	Get_VecCell();

	void		All_ClearNavigation();
	void		Sel_ClearNavigation(const _int& iCellIdx);

	void		Set_PickObj(const _vec3& vPos);
	void		Set_POINT(const _int& iIdx, const _vec3& vPickPos, const _vec3& vChangePos);
	void		Set_ObjectCollBox(_bool View);

	_uint		Pick_Navigation(const _vec3* vPickPos);
	_bool		Pick_Dist_Short(const _vec3* vPickPos, _vec3& vGetPos);

	void		Obj_WireFrame(_bool View);


	CGameObject*	Pick_Mesh(const _vec3* vPickPos, _ulong uX, _ulong uY);

	CGameObject*	Get_SelObject();

	void		Picking_Object(CGameObject* pObject);
	void		All_PickingObject(_bool Pick);

	void		Set_SelObjectTrans(_vec3* vChangePos, const _uint& idx);
	void		Set_DeleteSelObject();

private:
	HRESULT		Ready_LogoComponent(void);
	HRESULT		Ready_Texture(void);
	HRESULT		Ready_Layer_Camera(const _tchar* pLayerTag, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	HRESULT		Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_Navigation(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_Mouse(const _tchar* pLayerTag);

private:
	CD3DFont*			m_pD3dFont = nullptr;
	CPicking*			m_pPickingCom = nullptr;

	CGameObject*		m_pSelObject = nullptr;
	_char					m_szFPS[32] = "";
	_float					m_fFPS = 0.f;
	_float					m_fTimer = 0.f;


	_vec3				m_vNaviPos[3];
public:
	static CToolScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) override;
};

