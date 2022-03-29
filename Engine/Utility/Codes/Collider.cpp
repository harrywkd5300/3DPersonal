#include "..\Headers\Collider.h"

USING(Engine)

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_vMin(0.f, 0.f, 0.f)
	, m_vMax(0.f, 0.f, 0.f)
	, m_vOriginMin(0.f, 0.f, 0.f)
	, m_vOriginMax(0.f, 0.f, 0.f)
	, m_isColl(false)
	, m_pTransform(nullptr)
	, m_eType(TYPE_AABB)
	, m_fRadios(0.f)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CVIBuffer(rhs)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
	, m_vOriginMin(rhs.m_vOriginMin)
	, m_vOriginMax(rhs.m_vOriginMin)
	, m_isColl(rhs.m_isColl)
	, m_pTransform(nullptr)
	, m_eType(rhs.m_eType)
	, m_fRadios(rhs.m_fRadios)
{

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		m_pTexture[i] = rhs.m_pTexture[i];
		m_pTexture[i]->AddRef();
	}

}

void CCollider::Set_ColliderSetting(TYPE eType, const _vec3 * vMin, const _vec3 * vMax, const CTransform * pTransform, const _vec3* vRot)
{
	matScale.Identity();

	m_eType = eType;

	m_pTransform = pTransform;

	m_vMin = *vMin;
	m_vMax = *vMax;

	m_vRot = *vRot;

	matStartMatrix.Identity();

	matStartMatrix = *matStartMatrix.Identity();

	_matrix		matRotaX = *matRotaX.Identity();
	_matrix		matRotaY = *matRotaY.Identity();
	_matrix		matRotaZ = *matRotaZ.Identity();

	if (m_vRot.x != 0)
		D3DXMatrixRotationX(&matRotaX, D3DXToRadian(m_vRot.x));   // 로드 하는 모든 메시에 X축으로 180도 회전시켜서 그린다.
	if (m_vRot.y != 0)
		D3DXMatrixRotationY(&matRotaY, D3DXToRadian(m_vRot.y));   // 로드 하는 모든 메시에 Y축으로 180도 회전시켜서 그린다.
	if (m_vRot.z != 0)
		D3DXMatrixRotationZ(&matRotaZ, D3DXToRadian(m_vRot.z));   // 로드 하는 모든 메시에 Z축으로 180도 회전시켜서 그린다.

	matStartMatrix = matRotaX * matRotaY * matRotaZ;

	D3DXVec3TransformCoord(&m_vMin, &m_vMin, &matStartMatrix);
	D3DXVec3TransformCoord(&m_vMax, &m_vMax, &matStartMatrix);

	if (m_vMin.x < m_vMax.x)
	{
		m_vOriginMin.x = m_vMin.x;
		m_vOriginMax.x = m_vMax.x;
	}
	else
	{
		m_vOriginMin.x = m_vMax.x;
		m_vOriginMax.x = m_vMin.x;
	}

	if (m_vMin.y < m_vMax.y)
	{
		m_vOriginMin.y = m_vMin.y;
		m_vOriginMax.y = m_vMax.y;
	}
	else
	{
		m_vOriginMin.y = m_vMax.y;
		m_vOriginMax.y = m_vMin.y;
	}

	if (m_vMin.z < m_vMax.z)
	{
		m_vOriginMin.z = m_vMin.z;
		m_vOriginMax.z = m_vMax.z;
	}
	else
	{
		m_vOriginMin.z = m_vMax.z;
		m_vOriginMax.z = m_vMin.z;
	}

	if (m_eType == TYPE_CIRCLE)
	{
		_matrix matTmp;
		matTmp.Identity();
		matTmp = matScale *m_Circle;

		D3DXVec3TransformCoord(&m_vCircleMin, &m_vOriginMin, &matTmp);
		D3DXVec3TransformCoord(&m_vCircleMax, &m_vOriginMax, &matTmp);

		if (m_vCircleMin.x < m_vCircleMax.x)
		{
			m_vOriCircleMin.x = m_vCircleMin.x;
			m_vOriCircleMax.x = m_vCircleMax.x;
		}
		else
		{
			m_vOriCircleMin.x = m_vCircleMax.x;
			m_vOriCircleMax.x = m_vCircleMin.x;
		}

		if (m_vCircleMin.y < m_vCircleMax.y)
		{
			m_vOriCircleMin.y = m_vCircleMin.y;
			m_vOriCircleMax.y = m_vCircleMax.y;
		}
		else
		{
			m_vOriCircleMin.y = m_vCircleMax.y;
			m_vOriCircleMax.y = m_vCircleMin.y;
		}

		if (m_vCircleMin.z < m_vCircleMax.z)
		{
			m_vOriCircleMin.z = m_vCircleMin.z;
			m_vOriCircleMax.z = m_vCircleMax.z;
		}
		else
		{
			m_vOriCircleMin.z = m_vCircleMax.z;
			m_vOriCircleMax.z = m_vCircleMin.z;
		}
		_vec3 vRadio = (_vec3)matTmp.m[3] - m_vOriCircleMax;
		m_fRadios = D3DXVec3Length(&vRadio);

	}
	else
	{
		m_fRadios = m_vOriginMax.z;
	}
}

HRESULT CCollider::Ready_Colider()
{
	m_iVertexSize = sizeof(VTXTEX);
	m_iVertexCnt = 8;

	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = 12;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if (nullptr == m_pVB)
		return E_FAIL;

	VTXTEX*			pVertex = nullptr;


	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	pVertex[0].vTexUV = _vec2(0.f, 0.0f);

	pVertex[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertex[1].vTexUV = _vec2(0.f, 0.0f);

	pVertex[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertex[2].vTexUV = _vec2(0.f, 0.0f);

	pVertex[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].vTexUV = _vec2(0.f, 0.0f);

	// 뒷면
	pVertex[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].vTexUV = _vec2(0.f, 0.0f);

	pVertex[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertex[5].vTexUV = _vec2(0.f, 0.0f);

	pVertex[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertex[6].vTexUV = _vec2(0.f, 0.0f);

	pVertex[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	pVertex[7].vTexUV = _vec2(0.f, 0.0f);

	m_pVB->Unlock();

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// +x 
	pIndex[0]._0 = 1; pIndex[0]._1 = 5; pIndex[0]._2 = 6;
	pIndex[1]._0 = 1; pIndex[1]._1 = 6; pIndex[1]._2 = 2;

	// -x 
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;

	// +y
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;

	// -y 
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;

	// +z
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;

	// -z 
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;

	m_pIB->Unlock();


	for (size_t i = 0; i < COLLIDER_END; ++i)
	{
		if (FAILED(D3DXCreateTexture(m_pGraphicDev, 100, 100, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i])))
			return E_FAIL;
	}

	D3DLOCKED_RECT		Lock_Rect;

	m_pTexture[COLLIDER_COL]->LockRect(0, &Lock_Rect, nullptr, 0);

	for (size_t i = 0; i < 100; ++i)
	{
		for (size_t j = 0; j < 100; ++j)
		{
			_uint iIndex = i * 100 + j;

			((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		}
	}

	m_pTexture[COLLIDER_COL]->UnlockRect(0);

	m_pTexture[COLLIDER_NOCOL]->LockRect(0, &Lock_Rect, nullptr, 0);

	for (size_t i = 0; i < 100; ++i)
	{
		for (size_t j = 0; j < 100; ++j)
		{
			_uint iIndex = i * 100 + j;

			((_ulong*)Lock_Rect.pBits)[iIndex] = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
		}
	}
	m_pTexture[COLLIDER_NOCOL]->UnlockRect(0);

	//D3DXSaveTextureToFile(L"../Bin/Tmp.jpg", D3DXIFF_JPG, m_pTexture[0], nullptr);

	return NOERROR;
}

void CCollider::Render_Buffer()
{
#ifdef _DEBUG
	if (nullptr == m_pVB)
		return;

	VTXTEX*		pVertex = nullptr;

	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(m_vOriginMin.x, m_vOriginMax.y, m_vOriginMin.z);
	pVertex[1].vPosition = _vec3(m_vOriginMax.x, m_vOriginMax.y, m_vOriginMin.z);
	pVertex[2].vPosition = _vec3(m_vOriginMax.x, m_vOriginMin.y, m_vOriginMin.z);
	pVertex[3].vPosition = _vec3(m_vOriginMin.x, m_vOriginMin.y, m_vOriginMin.z);

	// 뒷면
	pVertex[4].vPosition = _vec3(m_vOriginMin.x, m_vOriginMax.y, m_vOriginMax.z);
	pVertex[5].vPosition = _vec3(m_vOriginMax.x, m_vOriginMax.y, m_vOriginMax.z);
	pVertex[6].vPosition = _vec3(m_vOriginMax.x, m_vOriginMin.y, m_vOriginMax.z);
	pVertex[7].vPosition = _vec3(m_vOriginMin.x, m_vOriginMin.y, m_vOriginMax.z);

	m_pVB->Unlock();

	_matrix matTmp;
	matTmp.Identity();

	if (TYPE_AABB == m_eType)
	{
		m_pTransform->Get_WorldMatrix_NoRot(&matTmp);
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);
	}
	else
	{
		m_pTransform->Get_WorldMatrix(&matTmp);
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);
	}

	if (true == m_isColl)
		m_pGraphicDev->SetTexture(0, m_pTexture[COLLIDER_COL]);
	else
		m_pGraphicDev->SetTexture(0, m_pTexture[COLLIDER_NOCOL]);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	CVIBuffer::Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	_matrix		matSrc;
	matSrc.Identity();

	m_pTransform->Get_WorldMatrix(&matSrc);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matSrc);


#endif // _DEBUG

}

void CCollider::Render_CircleBuffer(_vec3* vScale)
{
#ifdef _DEBUG
	if (nullptr == m_pVB)
		return;

	VTXTEX*		pVertex = nullptr;

	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(m_vOriginMin.x, m_vOriginMax.y, m_vOriginMin.z);
	pVertex[1].vPosition = _vec3(m_vOriginMax.x, m_vOriginMax.y, m_vOriginMin.z);
	pVertex[2].vPosition = _vec3(m_vOriginMax.x, m_vOriginMin.y, m_vOriginMin.z);
	pVertex[3].vPosition = _vec3(m_vOriginMin.x, m_vOriginMin.y, m_vOriginMin.z);

	// 뒷면
	pVertex[4].vPosition = _vec3(m_vOriginMin.x, m_vOriginMax.y, m_vOriginMax.z);
	pVertex[5].vPosition = _vec3(m_vOriginMax.x, m_vOriginMax.y, m_vOriginMax.z);
	pVertex[6].vPosition = _vec3(m_vOriginMax.x, m_vOriginMin.y, m_vOriginMax.z);
	pVertex[7].vPosition = _vec3(m_vOriginMin.x, m_vOriginMin.y, m_vOriginMax.z);

	m_pVB->Unlock();

	_matrix matTmp, matScale;
	matTmp.Identity();
	matScale.Identity();

	matScale._11 = vScale->x;
	matScale._22 = vScale->y;
	matScale._33 = vScale->z;

	if (TYPE_AABB == m_eType)
	{
		matTmp  = matScale *m_Circle;
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);
	}
	else
	{
		matTmp = matScale * m_Circle;
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);
	}

	if (true == m_isColl)
		m_pGraphicDev->SetTexture(0, m_pTexture[COLLIDER_COL]);
	else
		m_pGraphicDev->SetTexture(0, m_pTexture[COLLIDER_NOCOL]);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	CVIBuffer::Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	_matrix		matSrc;
	matSrc.Identity();

	m_pTransform->Get_WorldMatrix(&matSrc);

	matTmp = matTmp * m_Circle;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matSrc);


#endif // _DEBUG
}

_bool CCollider::Collision_AABB(CCollider * pTargetCollider)
{
	_vec3		vSourMin, vSourMax;
	_vec3		vTargetMin, vTargetMax;

	_matrix matWorldNoRotMatrix;
	matWorldNoRotMatrix.Identity();

	m_pTransform->Get_WorldMatrix_NoRot(&matWorldNoRotMatrix);

	// 각 콜라이더의 min max를 월드스페이스로 옮겼다.
	D3DXVec3TransformCoord(&vSourMin, &m_vOriginMin, &matWorldNoRotMatrix);
	D3DXVec3TransformCoord(&vSourMax, &m_vOriginMax, &matWorldNoRotMatrix);

	_matrix matTarget_WorldNoRotMatrix;

	pTargetCollider->Get_WorldMatrix_NotRot(&matTarget_WorldNoRotMatrix);

	D3DXVec3TransformCoord(&vTargetMin, &pTargetCollider->Get_OriginMin(), &matTarget_WorldNoRotMatrix);
	D3DXVec3TransformCoord(&vTargetMax, &pTargetCollider->Get_OriginMax(), &matTarget_WorldNoRotMatrix);

	// x 축 선상에서 바라봤을때 
	if (max(vSourMin.x, vTargetMin.x) > min(vSourMax.x, vTargetMax.x))
		goto notcoll;

	// y 축 선상에서 바라봤을때 
	if (max(vSourMin.y, vTargetMin.y) > min(vSourMax.y, vTargetMax.y))
		goto notcoll;

	// z 축 선상에서 바라봤을때 
	if (max(vSourMin.z, vTargetMin.z) > min(vSourMax.z, vTargetMax.z))
		goto notcoll;

	//pTargetCollider->Set_Coll(true);
	//m_isColl = true;

	pTargetCollider->Set_TargetNormal(&_vec3(0.f, 0.f, 0.f));
	Set_TargetNormal(&_vec3(0.f, 0.f, 0.f));

	if (vTargetMax.z - 0.1f < vSourMin.z && !m_bNext)
	{
		pTargetCollider->Set_TargetNormal(&_vec3(0.f, 0.f, -1.f));
		Set_TargetNormal(&_vec3(0.f, 0.f, 1.f));
	}
	else if (vTargetMin.z + 0.1f > vSourMax.z && !m_bNext)
	{
		pTargetCollider->Set_TargetNormal(&_vec3(0.f, 0.f, 1.f));
		Set_TargetNormal(&_vec3(0.f, 0.f, -1.f));
	}
	else if (vTargetMax.z > vSourMin.z && vTargetMin.z < vSourMax.z)
	{
		m_bNext = true;

		if (vTargetMax.x < vSourMax.x)
		{
			pTargetCollider->Set_TargetNormal(&_vec3(-1.f, 0.f, 0.f));
			Set_TargetNormal(&_vec3(1.f, 0.f, 0.f));
		}
		else
		{
			pTargetCollider->Set_TargetNormal(&_vec3(1.f, 0.f, 0.f));
			Set_TargetNormal(&_vec3(-1.f, 0.f, 0.f));
		}
	}

	return true;

notcoll:
	//pTargetCollider->Set_Coll(false);
	//m_isColl = false;
	m_bNext = false;
	return false;
}

_bool CCollider::Collision_OBB(CCollider * pTargetCollider)
{
	OBB				tOBB[2];

	Set_Point(tOBB, &m_vOriginMin, &m_vOriginMax);
	Set_Point(tOBB + 1, &pTargetCollider->Get_OriginMin(), &pTargetCollider->Get_OriginMax());

	_matrix matWorld, matTargetWorld;
	m_pTransform->Get_WorldMatrix(&matWorld);
	pTargetCollider->Get_WorldMatrix(&matTargetWorld);

	// 아홉개의 점을 월드스페이스로 옮겼다.
	for (size_t i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tOBB[0].vPoint[i], &tOBB[0].vPoint[i], &matWorld);
		D3DXVec3TransformCoord(&tOBB[1].vPoint[i], &tOBB[1].vPoint[i], &matTargetWorld);
	}
	D3DXVec3TransformCoord(&tOBB[0].vCenter, &tOBB[0].vCenter, &matWorld);
	D3DXVec3TransformCoord(&tOBB[1].vCenter, &tOBB[1].vCenter, &matTargetWorld);

	for (size_t i = 0; i < 2; i++)
		Set_Axis(tOBB + i);

	_float		fDistance[3];

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[0]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[1]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[2]));

			fDistance[1] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[0]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[1]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[2]));

			_vec3		vCenter = tOBB[1].vCenter - tOBB[0].vCenter;
			fDistance[2] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &vCenter));

			if (fDistance[0] + fDistance[1] < fDistance[2])
			{
				return false;
			}
		}
	}

	return true;
}

_bool CCollider::Collision_CIRCLE(CCollider * pTargetCollider)
{
	_float fSourRadious = m_fRadios;
	_float fDestRadious = pTargetCollider->Get_Radous();

	_matrix matWorldNoRotMatrix;
	matWorldNoRotMatrix.Identity();

	matWorldNoRotMatrix = matScale * m_Circle;

	_matrix matTarget_WorldNoRotMatrix;

	pTargetCollider->Get_WorldMatrix_NotRot(&matTarget_WorldNoRotMatrix);

	_vec3 vDist = (_vec3)matWorldNoRotMatrix.m[3] - (_vec3)matTarget_WorldNoRotMatrix.m[3];

	_float fDIst = D3DXVec3Length(&vDist);

	if (fDIst >= fSourRadious + fDestRadious)
	{
		/*m_isColl = false;
		pTargetCollider->Set_Coll(false);*/
		return false;
	}

	//m_isColl = true;
	//pTargetCollider->Set_Coll(true);
	return true;
}

void CCollider::Set_Radius(CCollider * pTargetCollider)
{
	_vec3		vSourMin, vSourMax;
	_vec3		vTargetMin, vTargetMax;

	_matrix matWorldNoRotMatrix;
	matWorldNoRotMatrix.Identity();

	m_pTransform->Get_WorldMatrix_NoRot(&matWorldNoRotMatrix);

	// 각 콜라이더의 min max를 월드스페이스로 옮겼다.
	D3DXVec3TransformCoord(&vSourMin, &m_vOriginMin, &matWorldNoRotMatrix);
	D3DXVec3TransformCoord(&vSourMax, &m_vOriginMax, &matWorldNoRotMatrix);

	_matrix matTarget_WorldNoRotMatrix;

	pTargetCollider->Get_WorldMatrix_NotRot(&matTarget_WorldNoRotMatrix);

	D3DXVec3TransformCoord(&vTargetMin, &pTargetCollider->Get_OriginMin(), &matTarget_WorldNoRotMatrix);
	D3DXVec3TransformCoord(&vTargetMax, &pTargetCollider->Get_OriginMax(), &matTarget_WorldNoRotMatrix);

	if (vSourMax.x < vTargetMax.x && vSourMax.z > vTargetMin.z)
		m_vRadius = vTargetMin - vSourMax;
	else if (vSourMax.x > vTargetMax.x && vSourMax.z > vTargetMin.z)
	{
		_vec3 vNSourMax = vSourMax;
		vNSourMax.x = vSourMin.x;
		_vec3 vNDstMax = vTargetMax;
		vNDstMax.z = vTargetMin.z;
		m_vRadius = vNDstMax - vNSourMax;
	}
	else if (vSourMax.x > vTargetMax.x && vSourMax.z > vTargetMax.z)
		m_vRadius = vTargetMax - vSourMin;
	else
	{
		_vec3 vNSourMax = vSourMax;
		vNSourMax.z = vSourMin.z;
		_vec3 vNDstMax = vTargetMax;
		vNDstMax.x = vTargetMin.x;
		m_vRadius = vNDstMax - vNSourMax;
	}
	m_vRadius.y = 0.f;
}
void CCollider::Set_Point(OBB * pOBB, const _vec3* pMin, const _vec3* pMax)
{
	// front
	pOBB->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pOBB->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pOBB->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pOBB->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	// back
	pOBB->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pOBB->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pOBB->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pOBB->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	// Center
	pOBB->vCenter = (*pMin + *pMax) * 0.5f;
}

void CCollider::Set_Axis(OBB * pOBB)
{
	// 
	pOBB->vAlign_Axis[0] = pOBB->vPoint[2] - pOBB->vPoint[3];
	pOBB->vAlign_Axis[1] = pOBB->vPoint[0] - pOBB->vPoint[3];
	pOBB->vAlign_Axis[2] = pOBB->vPoint[7] - pOBB->vPoint[3];

	for (size_t i = 0; i < 3; i++)
		D3DXVec3Normalize(&pOBB->vAlign_Axis[i], &pOBB->vAlign_Axis[i]);

	pOBB->vProj_Axis[0] = (pOBB->vPoint[5] + pOBB->vPoint[2]) * 0.5f - pOBB->vCenter;
	pOBB->vProj_Axis[1] = (pOBB->vPoint[5] + pOBB->vPoint[0]) * 0.5f - pOBB->vCenter;
	pOBB->vProj_Axis[2] = (pOBB->vPoint[5] + pOBB->vPoint[7]) * 0.5f - pOBB->vCenter;
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollider*	pInstance = new CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Colider()))
	{
		MSG_BOX("CCollider Created Failed!!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone()
{
	return new CCollider(*this);
}

_ulong CCollider::Free()
{
	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		Safe_Release(m_pTexture[i]);
	}
	return CVIBuffer::Free();
}
