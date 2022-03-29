#pragma once

#include "Component.h"


BEGIN(Engine)
class CCell;
class DLL_EXPORT CNavigation final : public CComponent
{
public:
	enum MOVETYPE { MOVE_NEIGHBOR, MOVE_OUT, MOVE_IN };

private:
	explicit CNavigation(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CNavigation(const CNavigation&	rhs);
	virtual ~CNavigation() = default;

public:
	HRESULT	Add_Cell(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC);
	void Set_Currrent_Index(const _uint& iIndex) { ICurrIndex = iIndex; }

private:
	vector<CCell*>				m_vecCell;
	typedef vector<CCell*>		VECCELL;

	_vec3						m_vCollNormal;

	_uint						ICurrIndex = 0;

private:
	void   Check_HaveNeighbor(const _vec3* vObjectPos);

public:
	HRESULT	Ready_Navigation(const _tchar* pFilePath);
	HRESULT	Reserve_Navigation(const _uint& iNumCell);
	HRESULT Ready_Neighbor_Cell(void);
	void	Render_Navigation();

	void	Correct_Navigation(CCell* pCell);

	_uint	Picking_Navigation(const _vec3* PickPos);
	void	All_NotSelectCell();

	void	All_Clear();
	void	Select_Clear(const _int& CellIdx);
	void	Set_SelCellPoint(const _int& idx, const _vec3& vPickPos, const _vec3& vChangePos);

public:
	_bool	Check_Move(const _vec3* vObjectPos);

	MOVETYPE Check_Navigation(const _vec3* pResultPos);
	MOVETYPE Check_SlideNavigation(const _vec3* pResultPos);

	_bool	Dist_Short(const _vec3 * PickPos, _vec3& vGetPos);

	vector<CCell*>*		Get_VecCell();
	CCell*				Get_Cell(const _uint& iIdx);

	_vec3*	Get_CollNormal() { return &m_vCollNormal; }

	const _vec3*  Get_CurrCellPos(const _uint& iIdx);

	void  Get_NeghbirIndex(_vec3* vOut);


public:
	virtual CComponent* Clone(void);
	static CNavigation* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath = nullptr);

private:
	virtual _ulong Free(void);
};

END