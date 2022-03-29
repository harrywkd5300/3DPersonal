#pragma once

#include "Base.h"
#include "Engine_Defines.h"
#include "Navigation.h"



BEGIN(Engine)

class CCube_Texture;
class CCell final : public CBase
{

public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum DIRECTION { DIR_AB, DIR_BC, DIR_CA, DIR_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCell() = default;

public:
	HRESULT Ready_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC);
	_bool Check_Neighbor(const _vec3* pSour, const _vec3* pDest, CCell* pCell);
	void	Render_Cell();

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	_vec3				m_vPoint[POINT_END];
	_vec3				m_vDirection[DIR_END];
	_vec3				m_vNormal[DIR_END];
	CCell*				m_pNeighbor[DIR_END];
private:
	_uint				m_iIndex = 0;
	LPD3DXLINE			m_pLine = nullptr;
	_float				m_fWhite = 1.f;

public: // Getter
	_vec3* Get_Point(POINT ePoint)  { return &m_vPoint[ePoint]; }
	_vec3* Get_NormalVec(DIRECTION iDir) { return &m_vNormal[iDir]; }
	CCell*	Get_NeighborCell(const _uint& iIdx) { return m_pNeighbor[iIdx]; }

	_uint				Get_Index() { return m_iIndex; }
	_uint				Get_NeighborIndex(const _uint& iIdx);

	CNavigation::MOVETYPE Check_Cell(const _vec3* pResultPos, _uint* pCurrentIdx, _vec3* vNormal);
	CNavigation::MOVETYPE Check_SlideCell(const _vec3* pResultPos, _uint* pCurrentIdx, _vec3* vNormal);

public: // Setter
	void				Set_Neighbor(_uint dir, CCell* NeighborCell);
	void				Set_Iindex(_uint iIndex) { m_iIndex = iIndex; }
	void				Set_DirectionNormal(const _vec3* pDirAB, const _vec3* pDirBC, const _vec3* pDirCA);
	void				Set_PickingCell_LineWhite();
	void				Set_PickingCell_LineBlack();
	void				Set_LineColor(const _float& Color) { m_fWhite = Color; }
	void				Set_Point(POINT ePoint, const _vec3& vChangePos);



public:
	static CCell* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC);

private:
	virtual _ulong Free(void);
};

END