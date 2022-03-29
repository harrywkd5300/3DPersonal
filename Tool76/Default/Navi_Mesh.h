#pragma once

#include "gameObject.h"
#include "Tool_Define.h"

namespace Engine
{
	class CRenderer;
	//class CNavigation;
	//class CCell;
}

#include "Navigation.h"
#include "cell.h"


class CNavi_Mesh final : public CGameObject
{
private:
	explicit CNavi_Mesh(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual ~CNavi_Mesh() = default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	HRESULT				Ready_Component(void);

private:
	CNavigation*		m_pNavigation  = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	void		Make_NaviMesh(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC);
	void		Reserve_Navigation(const _ulong& Size);
	vector<CCell*>*		Get_VecCell();

	void		All_Clear();
	void		Select_Delete(const _int& iCellIdx);
	_uint		Picking_Navigation(const _vec3* vPickPos);
	_bool		Picking_Dist_Short(const _vec3* vPickPos, _vec3& vGetPos);

	void		Set_PoInt(const _int iIdx, const _vec3& vClickPos, const _vec3 vChangePos);

public:
	static CNavi_Mesh* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free() final;

};


