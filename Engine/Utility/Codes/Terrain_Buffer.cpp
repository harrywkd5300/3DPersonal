#include "..\Headers\Terrain_Buffer.h"

USING(Engine)

CTerrain_Buffer::CTerrain_Buffer(LPDIRECT3DDEVICE9	pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

CTerrain_Buffer::CTerrain_Buffer(const CTerrain_Buffer & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CTerrain_Buffer::Ready_Buffer(const _tchar* pHeightMap, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _float & fInterval)
{
	m_iVertexSize = sizeof(VTXTEXNORMAL);
	m_iVertexCnt = iVtxCntX * iVtxCntZ;
	m_iVertexCntX = iVtxCntX;
	m_iVertexCntZ = iVtxCntZ;
	m_fVertexItv = fInterval;
	m_pVertexPos = new _vec3[m_iVertexCnt];
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = (iVtxCntX - 1) * (iVtxCntZ - 1) * 2;

	_ulong*			pPixel;

	if (nullptr != pHeightMap)
	{
		HANDLE hFile = 0;

		hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		BITMAPFILEHEADER	fh;
		BITMAPINFOHEADER	ih;

		_ulong				dwByte = 0;

		ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
		if (0 == dwByte)
			return E_FAIL;

		ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);
		if (0 == dwByte)
			return E_FAIL;

		_ulong NumPixel = ih.biWidth * ih.biHeight;

		pPixel = new _ulong[NumPixel];

		ReadFile(hFile, pPixel, sizeof(_ulong) * NumPixel, &dwByte, nullptr);

		if (0 == dwByte)
			return E_FAIL;

		CloseHandle(hFile);
	}


	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEXNORMAL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (size_t i = 0; i < iVtxCntZ; i++)
	{
		for (size_t j = 0; j < iVtxCntX; j++)
		{
			_uint		iIndex = i * iVtxCntX + j;

			if (nullptr != pHeightMap)
				pVertex[iIndex].vPosition = _vec3(j * fInterval, (pPixel[iIndex] & 0x000000ff) / 10.f, i * fInterval);
			else
				pVertex[iIndex].vPosition = _vec3(j * fInterval, 0.f, i * fInterval);

			m_pVertexPos[iIndex] = pVertex[iIndex].vPosition;
			pVertex[iIndex].vNormal = _vec3(0.0f, 0.f, 0.f);
			pVertex[iIndex].vTexUV = _vec2(j / _float(iVtxCntX - 1), i / _float(iVtxCntZ - 1));
		}
	}



	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_uint			iIndex = 0;

	for (size_t i = 0; i < iVtxCntZ - 1; ++i)
	{
		for (size_t j = 0; j < iVtxCntX - 1; ++j)
		{
			_uint		iVtxIdx = i * iVtxCntX + j;
			_uint		iLT, iRT, iLB, iRB;

			// 현재 반복문에서 기준이되는 정점인덱스를 바탕으로 사각형 네꼭지점의 인덱스르 ㄹ구한다.
			iLT = iVtxIdx + iVtxCntX;
			iRT = iLT + 1;
			iLB = iVtxIdx;
			iRB = iLB + 1;

			// 오른쪽 위
			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRT;
			pIndex[iIndex]._2 = iRB;

			_vec3		vDest, vSour, vNormal;
			vDest = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			vSour = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;

			D3DXVec3Cross(&vNormal, &vSour, &vDest);

			pVertex[pIndex[iIndex]._0].vNormal += vNormal;
			pVertex[pIndex[iIndex]._1].vNormal += vNormal;
			pVertex[pIndex[iIndex]._2].vNormal += vNormal;
			++iIndex;

			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRB;
			pIndex[iIndex]._2 = iLB;

			vDest = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			vSour = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;

			D3DXVec3Cross(&vNormal, &vSour, &vDest);

			pVertex[pIndex[iIndex]._0].vNormal += vNormal;
			pVertex[pIndex[iIndex]._1].vNormal += vNormal;
			pVertex[pIndex[iIndex]._2].vNormal += vNormal;

			++iIndex;
		}
	}

	for (size_t i = 0; i < iVtxCntZ; i++)
	{
		for (size_t j = 0; j < iVtxCntX; j++)
		{
			_uint		iIndex = i * iVtxCntX + j;

			D3DXVec3Normalize(&pVertex[iIndex].vNormal, &pVertex[iIndex].vNormal);
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	if (nullptr != pHeightMap)
		Safe_Delete_Array(pPixel);

	return NOERROR;
}

void CTerrain_Buffer::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

void CTerrain_Buffer::HeightTerrain(const _vec3 * vPickPos, const _float& fBrushSize, const _float& fMaxHeight,const _float& fSize,  const _float fHeight)
{
	VTXTEXNORMAL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	size_t	m_fMinX = size_t(vPickPos->x - (fBrushSize * 0.5f));
	size_t	m_fMinZ = size_t(vPickPos->z - (fBrushSize * 0.5f));
	size_t	m_fMaxX = size_t(vPickPos->x + (fBrushSize * 0.5f));
	size_t	m_fMaxZ = size_t(vPickPos->z + (fBrushSize * 0.5f));

	for (size_t i = m_fMinZ; i < m_fMaxZ; ++i)
	{
		for (size_t j = m_fMinX; j < m_fMaxX; ++j)
		{
			_uint		iIndex = _uint(i * m_iVertexCntX + j);
			_vec3 vLeng = m_pVertexPos[iIndex] - *vPickPos;
			_float fLength = D3DXVec3Length(&vLeng);
			if (fLength <= (fBrushSize * 0.5f))
			{
				if (m_pVertexPos[iIndex].y < fMaxHeight && m_pVertexPos[iIndex].y >= 0)
				{
					_float Y = cos(D3DXToRadian((90.f * (fLength / (fBrushSize * 0.5f)))));

					if(fSize == 1.f)
						pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, m_pVertexPos[iIndex].y + Y * fHeight, i * m_fVertexItv);
					else if(fSize == -1.f)
						pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, m_pVertexPos[iIndex].y - Y * fHeight, i * m_fVertexItv);
					else if (fSize == 0.f)
						pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, 0.f, i * m_fVertexItv);
					else if (fSize == 2.f)
					{
						//_float X = cos(D3DXToRadian((90.f * (fLength / (fBrushSize * 0.5f)))));
						//pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, X * fHeight, i * m_fVertexItv);
					}
					else if (fSize == 3.f)
						pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, fMaxHeight , i * m_fVertexItv);

					m_pVertexPos[iIndex] = pVertex[iIndex].vPosition;
				}
				else if (m_pVertexPos[iIndex].y <= 0)
				{
					pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, 0.f, i * m_fVertexItv);
					m_pVertexPos[iIndex] = pVertex[iIndex].vPosition;
				}
				else
				{
					pVertex[iIndex].vPosition = _vec3(j * m_fVertexItv, fMaxHeight-0.01f , i * m_fVertexItv);
					m_pVertexPos[iIndex] = pVertex[iIndex].vPosition;
				}
			}
		}
	}

	m_pVB->Unlock();

}

void CTerrain_Buffer::LoadTerrain(const _vec3 * vPosition)
{
	VTXTEXNORMAL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (size_t i = 0; i < m_iVertexCntZ; i++)
	{
		for (size_t j = 0; j < m_iVertexCntX; j++)
		{
			_uint		iIndex = i * m_iVertexCntX + j;

			pVertex[iIndex].vPosition = vPosition[iIndex];
			m_pVertexPos[iIndex] = pVertex[iIndex].vPosition;
			pVertex[iIndex].vNormal = _vec3(0.0f, 0.f, 0.f);
			pVertex[iIndex].vTexUV = _vec2(j / _float(m_iVertexCntX - 1), i / _float(m_iVertexCntZ - 1));
		}
	}

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_uint			iIndex = 0;

	for (size_t i = 0; i < m_iVertexCntZ - 1; ++i)
	{
		for (size_t j = 0; j < m_iVertexCntX - 1; ++j)
		{
			_uint		iVtxIdx = i * m_iVertexCntX + j;
			_uint		iLT, iRT, iLB, iRB;

			// 현재 반복문에서 기준이되는 정점인덱스를 바탕으로 사각형 네꼭지점의 인덱스르 ㄹ구한다.
			iLT = iVtxIdx + m_iVertexCntX;
			iRT = iLT + 1;
			iLB = iVtxIdx;
			iRB = iLB + 1;

			// 오른쪽 위
			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRT;
			pIndex[iIndex]._2 = iRB;

			_vec3		vDest, vSour, vNormal, vTangent, vBiNormal;
			vDest = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			vSour = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;

			D3DXVec3Cross(&vNormal, &vSour, &vDest);

			pVertex[pIndex[iIndex]._0].vNormal += vNormal;
			pVertex[pIndex[iIndex]._1].vNormal += vNormal;
			pVertex[pIndex[iIndex]._2].vNormal += vNormal;

			//vTangent = _vec3(vNormal.y, -vNormal.x, 0.f);

			//pVertex[pIndex[iIndex]._0].vTangent += vTangent;
			//pVertex[pIndex[iIndex]._1].vTangent += vTangent;
			//pVertex[pIndex[iIndex]._2].vTangent += vTangent;

			//D3DXVec3Cross(&vBiNormal, &vNormal, &vTangent);

			//pVertex[pIndex[iIndex]._0].vBiNormal += vBiNormal;
			//pVertex[pIndex[iIndex]._1].vBiNormal += vBiNormal;
			//pVertex[pIndex[iIndex]._2].vBiNormal += vBiNormal;

			++iIndex;

			pIndex[iIndex]._0 = iLT;
			pIndex[iIndex]._1 = iRB;
			pIndex[iIndex]._2 = iLB;

			vDest = pVertex[pIndex[iIndex]._2].vPosition - pVertex[pIndex[iIndex]._1].vPosition;
			vSour = pVertex[pIndex[iIndex]._1].vPosition - pVertex[pIndex[iIndex]._0].vPosition;

			D3DXVec3Cross(&vNormal, &vSour, &vDest);

			pVertex[pIndex[iIndex]._0].vNormal += vNormal;
			pVertex[pIndex[iIndex]._1].vNormal += vNormal;
			pVertex[pIndex[iIndex]._2].vNormal += vNormal;

			//vTangent = _vec3(vNormal.y, -vNormal.x, 0.f);

			//pVertex[pIndex[iIndex]._0].vTangent += vTangent;
			//pVertex[pIndex[iIndex]._1].vTangent += vTangent;
			//pVertex[pIndex[iIndex]._2].vTangent += vTangent;

			//D3DXVec3Cross(&vBiNormal, &vNormal, &vTangent);

			//pVertex[pIndex[iIndex]._0].vBiNormal += vBiNormal;
			//pVertex[pIndex[iIndex]._1].vBiNormal += vBiNormal;
			//pVertex[pIndex[iIndex]._2].vBiNormal += vBiNormal;

			++iIndex;
		}
	}

	for (size_t i = 0; i < m_iVertexCntZ; i++)
	{
		for (size_t j = 0; j < m_iVertexCntZ; j++)
		{
			_uint		iIndex = i * m_iVertexCntZ + j;

			D3DXVec3Normalize(&pVertex[iIndex].vNormal, &pVertex[iIndex].vNormal);
			//D3DXVec3Normalize(&pVertex[iIndex].vTangent, &pVertex[iIndex].vTangent);
			//D3DXVec3Normalize(&pVertex[iIndex].vBiNormal, &pVertex[iIndex].vBiNormal);
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();
}

CComponent * CTerrain_Buffer::Clone(void)
{
	return new CTerrain_Buffer(*this);
}

CTerrain_Buffer * CTerrain_Buffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _float & fInterval, const _tchar* pHeightMap)
{
	CTerrain_Buffer *	pInstance = new CTerrain_Buffer(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(pHeightMap, iVtxCntX, iVtxCntZ, fInterval)))
	{
		MessageBox(0, L"CTerrain_Buffer Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CTerrain_Buffer::Free(void)
{
	return Engine::CVIBuffer::Free();
}
