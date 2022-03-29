#include "..\Headers\Navigation.h"

#include "Cell.h"

USING(Engine)

CNavigation::CNavigation(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
	, ICurrIndex(rhs.ICurrIndex)
{
	_uint iSize = m_vecCell.size();

	for (size_t i = 0; i < iSize; ++i)
		m_vecCell[i]->AddRef();

}

HRESULT CNavigation::Add_Cell(const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC)
{
	CCell*		pCell = CCell::Create(m_pGraphicDev, pPointA, pPointB, pPointC);
	if (nullptr == pCell)
		return E_FAIL;

	
	_uint iSize = m_vecCell.size();

	All_NotSelectCell();

	pCell->Set_PickingCell_LineBlack();

	Correct_Navigation(pCell);

	pCell->Set_Iindex(m_vecCell.size());

	m_vecCell.push_back(pCell);

	return NOERROR;
}

void CNavigation::Check_HaveNeighbor(const _vec3 * vObjectPos)
{
	if (m_vecCell.empty())
		return;

	for (size_t i = 0; i < 3; ++i)
	{
		if (nullptr == m_vecCell[ICurrIndex]->Get_NeighborCell(i))
			continue;

		_vec3 ObjectDir[3];
		ObjectDir[0] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Point(CCell::POINT_C);

		_vec3 Normal[CCell::DIR_END];
		Normal[CCell::DIR_AB] = *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_NormalVec(CCell::DIR_AB);
		Normal[CCell::DIR_BC] = *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_NormalVec(CCell::DIR_BC);
		Normal[CCell::DIR_CA] = *m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_NormalVec(CCell::DIR_CA);

		for (int j = 0; j < 3; ++j)
		{
			D3DXVec3Normalize(&ObjectDir[j], &ObjectDir[j]);

			float fDotResult = D3DXVec3Dot(&ObjectDir[j], &Normal[j]);

			if (fDotResult > 0.f)
			{
				break;
			}
			if (2 == j)
			{
				ICurrIndex = m_vecCell[ICurrIndex]->Get_NeighborIndex(i);
				return;
			}
		}
	}

}

HRESULT CNavigation::Ready_Navigation(const _tchar * pFilePath)
{
	if (nullptr == pFilePath)
		return NOERROR;

	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong			dwByte = 0;

	_ulong			dwNumCell = 0;

	ReadFile(hFile, &dwNumCell, sizeof(_ulong), &dwByte, nullptr);

	Reserve_Navigation(dwNumCell);

	for (size_t i = 0; i < dwNumCell; i++)
	{
		_vec3			vPoint[3];

		ReadFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			return E_FAIL;

		//vPoint[0].y = 0.f;
		//vPoint[1].y = 0.f;
		//vPoint[2].y = 0.f;

		Add_Cell(vPoint, vPoint + 1, vPoint + 2);
	}

	Ready_Neighbor_Cell();

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CNavigation::Reserve_Navigation(const _uint & iNumCell)
{
	if (false == m_vecCell.empty())
		return E_FAIL;

	m_vecCell.reserve(iNumCell);

	return NOERROR;
}

HRESULT CNavigation::Ready_Neighbor_Cell(void)
{
	_uint			iSize = m_vecCell.size();

	for (size_t i = 0; i < iSize; i++)
	{
		for (size_t j = 0; j < iSize; j++)
		{
			if (m_vecCell[i] == m_vecCell[j])
				continue;

			// Check_Neighbor(이웃에 유무에 대한 체크를 하고 멤버에 대입하낟)
			if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_A), m_vecCell[i]->Get_Point(CCell::POINT_B), m_vecCell[i]))
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);

			else if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_B), m_vecCell[i]->Get_Point(CCell::POINT_C), m_vecCell[i]))
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);

			else if (true == m_vecCell[j]->Check_Neighbor(m_vecCell[i]->Get_Point(CCell::POINT_C), m_vecCell[i]->Get_Point(CCell::POINT_A), m_vecCell[i]))
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);

		}
	}
	return NOERROR;
}

void CNavigation::Render_Navigation()
{
	if (m_vecCell.empty())
		return;

	auto iter = m_vecCell.begin();
	for (; iter != m_vecCell.end(); ++iter)
	{
		if(nullptr != (*iter))
			(*iter)->Render_Cell();
	}
}

void CNavigation::Correct_Navigation(CCell* pCell)
{
	_vec3 Point[3] = { *pCell->Get_Point(CCell::POINT_A), *pCell->Get_Point(CCell::POINT_B), *pCell->Get_Point(CCell::POINT_C) };

	_vec3 Normal[3] = { *pCell->Get_NormalVec(CCell::DIR_AB), *pCell->Get_NormalVec(CCell::DIR_BC), *pCell->Get_NormalVec(CCell::DIR_CA) };
	
	_vec3 vCompairPos = _vec3((Point[0].x + Point[1].x + Point[2].x) / 3.f,
							  (Point[0].y + Point[1].y + Point[2].y) / 3.f,
						      (Point[0].z + Point[1].z + Point[2].z) / 3.f);

	_vec3 ObjectDir[3];
	ObjectDir[0] = vCompairPos - Point[0];
	ObjectDir[1] = vCompairPos - Point[1];
	ObjectDir[2] = vCompairPos - Point[2];

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3Normalize(&ObjectDir[i], &ObjectDir[i]);

		float fDotResult = D3DXVec3Dot(&ObjectDir[i], &Normal[i]);

		if (fDotResult > 0.f)
		{
			_vec3 vDir[CCell::DIR_END];
			vDir[CCell::DIR_AB] = *pCell->Get_Point(CCell::POINT_A) - *pCell->Get_Point(CCell::POINT_B);
			vDir[CCell::DIR_BC] = *pCell->Get_Point(CCell::POINT_B) - *pCell->Get_Point(CCell::POINT_C);
			vDir[CCell::DIR_CA] = *pCell->Get_Point(CCell::POINT_C) - *pCell->Get_Point(CCell::POINT_A);

			pCell->Set_DirectionNormal(&vDir[CCell::DIR_AB], &vDir[CCell::DIR_BC], &vDir[CCell::DIR_CA]);
			return;
		}
	}

	return;
}

_uint CNavigation::Picking_Navigation(const _vec3 * PickPos)
{
	if (m_vecCell.empty())
		return -1;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		_vec3 ObjectDir[3];
		ObjectDir[0] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_C);

		_vec3 Normal[CCell::DIR_END];
		Normal[CCell::DIR_AB] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_AB);
		Normal[CCell::DIR_BC] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_BC);
		Normal[CCell::DIR_CA] = *m_vecCell[i]->Get_NormalVec(CCell::DIR_CA);

		for (int j = 0; j < 3; ++j)
		{
			D3DXVec3Normalize(&ObjectDir[j], &ObjectDir[j]);

			float fDotResult = D3DXVec3Dot(&ObjectDir[j], &Normal[j]);

			if (fDotResult > 0.f)
				break;

			if (2 == j)
			{
				All_NotSelectCell();
				m_vecCell[i]->Set_LineColor(0.f);
				return i;
			}
		}
	}

	return -1;
}

void CNavigation::All_NotSelectCell()
{
	for (size_t i = 0; i < m_vecCell.size(); ++i)
		m_vecCell[i]->Set_LineColor(1.f);
}

void CNavigation::All_Clear()
{
	for (size_t i = 0; i < m_vecCell.size(); ++i)
		Safe_Release(m_vecCell[i]);

	m_vecCell.clear();
}

void CNavigation::Select_Clear(const _int & CellIdx)
{
	Safe_Release(m_vecCell[CellIdx]);
	m_vecCell.erase(m_vecCell.begin() + CellIdx);
}

void CNavigation::Set_SelCellPoint(const _int & idx, const _vec3 & vPickPos, const _vec3& vChangePos)
{
	if (m_vecCell.empty())
		return;

	_vec3 ObjectDir[3];
	ObjectDir[0] = vPickPos - *m_vecCell[idx]->Get_Point(CCell::POINT_A);
	ObjectDir[1] = vPickPos - *m_vecCell[idx]->Get_Point(CCell::POINT_B);
	ObjectDir[2] = vPickPos - *m_vecCell[idx]->Get_Point(CCell::POINT_C);

	_float fDist[3];
	for (size_t i = 0; i < 3; ++i)
		fDist[i] = D3DXVec3Length(&ObjectDir[i]);

	if (fDist[0] > fDist[1])
	{
		if (fDist[1] > fDist[2])
			m_vecCell[idx]->Set_Point(CCell::POINT_C, vChangePos);
		else
			m_vecCell[idx]->Set_Point(CCell::POINT_B, vChangePos);
	}
	else
	{
		if (fDist[0] > fDist[2])
			m_vecCell[idx]->Set_Point(CCell::POINT_C, vChangePos);
		else
			m_vecCell[idx]->Set_Point(CCell::POINT_A, vChangePos);
	}
}

_bool CNavigation::Check_Move(const _vec3 * vObjectPos)
{
	if (m_vecCell.empty())
		return true;

	Check_HaveNeighbor(vObjectPos);

	_vec3 ObjectDir[3];
	ObjectDir[0] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_Point(CCell::POINT_A);
	ObjectDir[1] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_Point(CCell::POINT_B);
	ObjectDir[2] = *vObjectPos - *m_vecCell[ICurrIndex]->Get_Point(CCell::POINT_C);

	_vec3 Normal[CCell::DIR_END];
	Normal[CCell::DIR_AB] = *m_vecCell[ICurrIndex]->Get_NormalVec(CCell::DIR_AB);
	Normal[CCell::DIR_BC] = *m_vecCell[ICurrIndex]->Get_NormalVec(CCell::DIR_BC);
	Normal[CCell::DIR_CA] = *m_vecCell[ICurrIndex]->Get_NormalVec(CCell::DIR_CA);

	m_vCollNormal = _vec3(0.f, 0.f, 0.f);

	for (int i = 0; i < 3; ++i)
	{
		D3DXVec3Normalize(&ObjectDir[i], &ObjectDir[i]);

		float fDotResult = D3DXVec3Dot(&ObjectDir[i], &Normal[i]);

		if (fDotResult > 0.f)
		{
			m_vCollNormal = Normal[i];
			return false;
		}
	}
	return true;
}

CNavigation::MOVETYPE CNavigation::Check_Navigation(const _vec3 * pResultPos)
{
	return m_vecCell[ICurrIndex]->Check_Cell(pResultPos, &ICurrIndex, &m_vCollNormal);
}

CNavigation::MOVETYPE CNavigation::Check_SlideNavigation(const _vec3 * pResultPos)
{
	return m_vecCell[ICurrIndex]->Check_SlideCell(pResultPos, &ICurrIndex, &m_vCollNormal);
}


_bool CNavigation::Dist_Short(const _vec3 * PickPos, _vec3 & vGetPos)
{
	if (m_vecCell.empty())
		return false;

	for (size_t i = 0; i < m_vecCell.size(); ++i)
	{
		_vec3 ObjectDir[3];
		ObjectDir[0] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_A);
		ObjectDir[1] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_B);
		ObjectDir[2] = *PickPos - *m_vecCell[i]->Get_Point(CCell::POINT_C);


		_float fDist[3];

		fDist[0] = D3DXVec3Length(&ObjectDir[0]);
		if (fDist[0] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_A);
			return true;
		}
		fDist[1] = D3DXVec3Length(&ObjectDir[1]);
		if (fDist[1] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_B);
			return true;
		}
		fDist[2] = D3DXVec3Length(&ObjectDir[2]);
		if (fDist[2] < 0.4f)
		{
			vGetPos = *m_vecCell[i]->Get_Point(CCell::POINT_C);
			return true;
		}
	}

	return false;
}

vector<CCell*>* CNavigation::Get_VecCell()
{
	return &m_vecCell;
}

CCell* CNavigation::Get_Cell(const _uint & iIdx)
{
	return m_vecCell[iIdx];
}

const _vec3 * CNavigation::Get_CurrCellPos(const _uint& iIdx)
{
	return m_vecCell[ICurrIndex]->Get_Point(CCell::POINT(iIdx));
}

void CNavigation::Get_NeghbirIndex(_vec3* vOut)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (nullptr != m_vecCell[ICurrIndex]->Get_NeighborCell(i))
		{
			if (0 == i)
				vOut->x = m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Index();
			else if (1 == i)
				vOut->y = m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Index();
			else
				vOut->z = m_vecCell[ICurrIndex]->Get_NeighborCell(i)->Get_Index();
		}
		else
		{
			if (0 == i)
				vOut->x = -1.f;
			else if (1 == i)
				vOut->y = -1.f;
			else
				vOut->z = -1.f;
		}
	}

	return;
}

CComponent * CNavigation::Clone(void)
{
	return new CNavigation(*this);
}

CNavigation * CNavigation::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath)
{
	CNavigation* pInstance = new CNavigation(pGraphicDev);

	if (FAILED(pInstance->Ready_Navigation(pFilePath)))
	{
		MSG_BOX("CNavigation Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CNavigation::Free(void)
{
	for (size_t i = 0; i < m_vecCell.size() ; ++i)
		Safe_Release(m_vecCell[i]);

	m_vecCell.clear();

	return CComponent::Free();;
}
