#include "..\Headers\Cell.h"

#include "Graphic_Device.h"

USING(Engine)

CCell::CCell(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_fWhite(1.f)
{
	m_pGraphicDev->AddRef();

	for (size_t i = 0; i < DIR_END; ++i)
	{
		m_pNeighbor[i] = nullptr;
	}

}

HRESULT CCell::Ready_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC)
{
	m_pLine = CGraphic_Device::GetInstance()->Get_Line();
	m_pLine->AddRef();

	m_vPoint[0] = *pPointA;
	m_vPoint[1] = *pPointB;
	m_vPoint[2] = *pPointC;

	m_vDirection[DIR_AB] = m_vPoint[POINT_B] - m_vPoint[POINT_A];
	m_vDirection[DIR_BC] = m_vPoint[POINT_C] - m_vPoint[POINT_B];
	m_vDirection[DIR_CA] = m_vPoint[POINT_A] - m_vPoint[POINT_C];

	m_vNormal[DIR_AB] = _vec3(-m_vDirection[DIR_AB].z, m_vDirection[DIR_AB].y, m_vDirection[DIR_AB].x);
	m_vNormal[DIR_BC] = _vec3(-m_vDirection[DIR_BC].z, m_vDirection[DIR_BC].y, m_vDirection[DIR_BC].x);
	m_vNormal[DIR_CA] = _vec3(-m_vDirection[DIR_CA].z, m_vDirection[DIR_CA].y, m_vDirection[DIR_CA].x);

	for (size_t i = 0; i < DIR_END; i++)
		D3DXVec3Normalize(&m_vNormal[i], &m_vNormal[i]);

	return NOERROR;
}

_bool CCell::Check_Neighbor(const _vec3 * pSour, const _vec3 * pDest, CCell * pCell)
{
	if (m_vPoint[POINT_A] == *pSour)
	{
		if (m_vPoint[POINT_B] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		else if (m_vPoint[POINT_C] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_B] == *pSour)
	{
		if (m_vPoint[POINT_A] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		else if (m_vPoint[POINT_C] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_C] == *pSour)
	{
		if (m_vPoint[POINT_A] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
		else if (m_vPoint[POINT_B] == *pDest)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}


	return false;
}

CNavigation::MOVETYPE CCell::Check_Cell(const _vec3 * pResultPos, _uint* pCurrentIdx, _vec3* vNormal)
{
	*vNormal = _vec3(0.f, 0.f, 0.f);

	for (size_t i = 0; i < DIR_END; i++)
	{
		_vec3	vSour = *pResultPos - m_vPoint[i];
		if (D3DXVec3Dot(D3DXVec3Normalize(&vSour, &vSour), &m_vNormal[i]) > 0)
		{
			if (nullptr == m_pNeighbor[i])
			{
				*vNormal = m_vNormal[i];
				return CNavigation::MOVE_OUT;
			}
			else
			{
				*pCurrentIdx = m_pNeighbor[i]->m_iIndex;
				return CNavigation::MOVE_NEIGHBOR;
			}

		}
	}
	return CNavigation::MOVE_IN;
}

CNavigation::MOVETYPE CCell::Check_SlideCell(const _vec3 * pResultPos, _uint* pCurrentIdx, _vec3* vNormal)
{
	for (size_t i = 0; i < DIR_END; i++)
	{
		_vec3	vSour = *pResultPos - m_vPoint[i];
		if (D3DXVec3Dot(D3DXVec3Normalize(&vSour, &vSour), &m_vNormal[i]) > 0)
		{
			if (nullptr == m_pNeighbor[i])
			{
				*vNormal = _vec3(0.f, 0.f, 0.f);
				return CNavigation::MOVE_OUT;
			}
			else
			{
				*pCurrentIdx = m_pNeighbor[i]->m_iIndex;
				return CNavigation::MOVE_NEIGHBOR;
			}

		}
	}
	return CNavigation::MOVE_IN;
}

void CCell::Render_Cell()
{
#ifdef _DEBUG

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	_vec3		vPoint[] = {
		m_vPoint[0],
		m_vPoint[1],
		m_vPoint[2],
		m_vPoint[0] };

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	for (size_t i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);
		if (vPoint[i].z < 0.0f)
			vPoint[i].z = 0.f;

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	_matrix			matTmp;

	m_pLine->Begin();

	m_pLine->DrawTransform(vPoint, 4, D3DXMatrixIdentity(&matTmp), D3DXCOLOR(m_fWhite, m_fWhite, m_fWhite, 1.f));


	m_pLine->End();

	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif	
}

_uint CCell::Get_NeighborIndex(const _uint & iIdx)
{
	if (nullptr == m_pNeighbor[iIdx])
		return m_iIndex;

	return m_pNeighbor[iIdx]->Get_Index();
}

void CCell::Set_Neighbor(_uint dir, CCell * NeighborCell)
{
	if (NeighborCell== nullptr)
		return;

	if (m_pNeighbor[dir] != NULL)
		return;

	m_pNeighbor[dir] = NeighborCell;
}

void CCell::Set_DirectionNormal(const _vec3 * pDirAB, const _vec3 * pDirBC, const _vec3 * pDirCA)
{
	m_vDirection[DIR_AB] = *pDirAB;
	m_vDirection[DIR_BC] = *pDirBC;
	m_vDirection[DIR_CA] = *pDirCA;

	m_vNormal[DIR_AB] = _vec3(-m_vDirection[DIR_AB].z, m_vDirection[DIR_AB].y, m_vDirection[DIR_AB].x);
	m_vNormal[DIR_BC] = _vec3(-m_vDirection[DIR_BC].z, m_vDirection[DIR_BC].y, m_vDirection[DIR_BC].x);
	m_vNormal[DIR_CA] = _vec3(-m_vDirection[DIR_CA].z, m_vDirection[DIR_CA].y, m_vDirection[DIR_CA].x);

	for (size_t i = 0; i < DIR_END; i++)
		D3DXVec3Normalize(&m_vNormal[i], &m_vNormal[i]);

}

void CCell::Set_PickingCell_LineBlack()
{
	m_fWhite = 0.f;
}

void CCell::Set_PickingCell_LineWhite()
{
	m_fWhite = 1.f;
}

void CCell::Set_Point(POINT ePoint, const _vec3 & vChangePos)
{
	m_vPoint[ePoint] = vChangePos;
}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC)
{
	CCell *		pInstance = new CCell(pGraphicDev);

	if (FAILED(pInstance->Ready_Cell(pPointA, pPointB, pPointC)))
	{
		MSG_BOX("CCell Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCell::Free(void)
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pLine);

	return 0;
}
