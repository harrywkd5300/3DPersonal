#include "..\Headers\Transform.h"

USING(Engine)

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
	, m_matScale(rhs.m_matScale)
	, m_matRotationX(rhs.m_matRotationX)
	, m_matRotationY(rhs.m_matRotationY)
	, m_matRotationZ(rhs.m_matRotationZ)
	, m_matTrans(rhs.m_matTrans)
	, m_matParent(rhs.m_matParent)
	, m_fDegreeX(rhs.m_fDegreeX)
	, m_fDegreeY(rhs.m_fDegreeY)
	, m_fDegreeZ(rhs.m_fDegreeZ)
{

}

void CTransform::Make_WorldMatrix()
{
	m_matWorld.Identity();

	m_matWorld = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * m_matTrans * m_matParent;

	//_vec3& vPos = (_vec3&)m_matWorld.m[INFO_POSITION];
	//vPos += (_vec3&)m_matTrans.m[INFO_POSITION];

	//D3DXVec3Normalize(&m_vLook, &m_vLook);

	//(_vec3&)m_matWorld.m[INFO_LOOK] = m_vLook;
	//_vec3& vRight = (_vec3&)m_matWorld.m[INFO_RIGHT];
	//_vec3& vUp = (_vec3&)m_matWorld.m[INFO_UP];

	//D3DXVec3Cross(&vRight, &_vec3(0, 1.f, 0), &m_vLook);
	//D3DXVec3Normalize(&vRight, &vRight);

	//D3DXVec3Cross(&vUp, &m_vLook, &vRight);
	//D3DXVec3Normalize(&vUp, &vUp);

	//m_matWorld = m_matScale * m_matWorld * m_matParent;

}

void CTransform::SetUp_OnGraphicDev(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void CTransform::Get_WorldMatrix(_matrix* matWorld, _matrix* matRot) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (nullptr == matRot)
		*matWorld = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * m_matTrans * m_matParent;
	else
		*matWorld = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * *matRot * m_matTrans * m_matParent;
}

void CTransform::Get_WorldMatrix_NoRot(_matrix* matWorld, _matrix* matRot) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_matrix matCopy = *matCopy.Identity();
	if (nullptr == matRot)
		matCopy = m_matScale * m_matTrans * m_matParent;
	else
		matCopy = m_matScale * *matRot * m_matTrans * m_matParent;

	*matWorld = matCopy;
}


_float CTransform::Compute_Distance(const _matrix * pWorldMatrix)
{
	_vec3	vDistance = *(_vec3*)&pWorldMatrix->m[INFO_POSITION][0] - *(_vec3*)&m_matWorld.m[INFO_POSITION][0];

	return D3DXVec3Length(&vDistance);
}

_float CTransform::Compute_Distance(const _vec3* vPosition)
{
	_vec3	vDistance = *vPosition - *(_vec3*)&m_matTrans.m[INFO_POSITION][0];

	return D3DXVec3Length(&vDistance);
}

void CTransform::Move_OnBuffer(Engine::CVIBuffer * pTargetBuffer)
{
	if (nullptr == pTargetBuffer)
		return;

	tuple<_uint, _uint, _float>	VertexCnt = pTargetBuffer->Get_VertexCntXZItv();

	const _vec3*	pVertexPos = pTargetBuffer->Get_VertexPos();

	_vec3			vPosition;
	memcpy(&vPosition, &m_matWorld.m[3][0], sizeof(_vec3));


	_int		iCurrentIndex = _int((vPosition.z) / get<2>(VertexCnt)) * (get<0>(VertexCnt)) + _int((vPosition.x) / get<2>(VertexCnt));

	_uint		iLT, iRT, iLB, iRB;

	iLT = iCurrentIndex + get<0>(VertexCnt);
	iRT = iLT + 1;
	iLB = iCurrentIndex;
	iRB = iLB + 1;

	_vec3 vDirection = pVertexPos[2] - pVertexPos[0];
	_vec3 vNormal = _vec3(-vDirection.z, 0.f, vDirection.x);
	D3DXVec3Normalize(&vNormal, &vNormal);

	_vec3 vTargetPos = vPosition - pVertexPos[0];
	D3DXVec3Normalize(&vTargetPos, &vTargetPos);

	_float fResult = D3DXVec3Dot(&vTargetPos, &vNormal);

	// 플레이어의 x와 z의 비율
	_float	fRatioX = (vPosition.x - pVertexPos[iLT].x) / get<2>(VertexCnt);  // Interval이 1인 경우에는 전체 길이를 의미한다. 
	_float	fRatioZ = (pVertexPos[iLT].z - vPosition.z) / get<2>(VertexCnt);  // Interval이 1인 경우에는 전체 길이를 의미한다. 

	_float	fTargetY = 0.f;


	// 따로 내적을 안쓰고하는법은 , 단, interval이 1인 경우에만 가능...., 정확한 방법은 내적으로 하는 법
	//if (fRatioX > fRatioZ)
	//	오른쪽 위 삼각형내
	//else
	//	왼쪽 아래 삼각형내
	if (fResult > 0.f)
		fTargetY = pVertexPos[iLT].y + (pVertexPos[iRT].y - pVertexPos[iLT].y) * fRatioX + ((pVertexPos[iRB]).y - pVertexPos[iRT].y) * fRatioZ;
	else
		fTargetY = pVertexPos[iLT].y + (pVertexPos[iLB].y - pVertexPos[iLT].y) * fRatioX + ((pVertexPos[iRB]).y - pVertexPos[iLB].y) * fRatioZ;
	Set_PositionY(fTargetY);
}

void CTransform::Move_OnNavigation(Engine::CNavigation * pNavigation)
{
	if (nullptr == pNavigation)
		return;

	_vec3 vPointA = *pNavigation->Get_CurrCellPos(0);
	_vec3 vPointB = *pNavigation->Get_CurrCellPos(1);
	_vec3 vPointC = *pNavigation->Get_CurrCellPos(2);

	_vec3			vPosition;
	memcpy(&vPosition, &m_matTrans.m[3][0], sizeof(_vec3));

	D3DXPLANE tCellPlane;

	D3DXPlaneFromPoints(&tCellPlane, &vPointA, &vPointB, &vPointC);

	_float fPosy = (tCellPlane.a * vPosition.x + tCellPlane.c * vPosition.z + tCellPlane.d) / tCellPlane.b;

	Set_PositionY(-fPosy);
}

void CTransform::Get_CurPositionY(Engine::CNavigation * pNavigation, _float* fY)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (nullptr == pNavigation)
		return;

	_vec3 vPointA = *pNavigation->Get_CurrCellPos(0);
	_vec3 vPointB = *pNavigation->Get_CurrCellPos(1);
	_vec3 vPointC = *pNavigation->Get_CurrCellPos(2);

	_vec3			vPosition;
	memcpy(&vPosition, &m_matTrans.m[3][0], sizeof(_vec3));

	D3DXPLANE tCellPlane;

	D3DXPlaneFromPoints(&tCellPlane, &vPointA, &vPointB, &vPointC);

	*fY = (tCellPlane.a * vPosition.x + tCellPlane.c * vPosition.z + tCellPlane.d) / tCellPlane.b;
}

HRESULT CTransform::Ready_Transform(void)
{
	m_matWorld.Identity();
	m_matWorld.Identity();
	m_matScale.Identity();
	m_matRotationX.Identity();
	m_matRotationY.Identity();
	m_matRotationZ.Identity();
	m_matTrans.Identity();
	m_matParent.Identity();

	m_vLook = _vec3(0.f, 0.f, 0.f);

	return NOERROR;
}

void CTransform::Update_Component(const _float & fTimeDelta)
{

}

CComponent * CTransform::Clone(void)
{
	return new CTransform(*this);
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform *		pInstance = new CTransform(pGraphicDev);

	if (FAILED(pInstance->Ready_Transform()))
	{
		MSG_BOX("CTransform Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CTransform::Free(void)
{
	return CComponent::Free();
}

_vec3 CTransform::Get_Look()
{
	_matrix		m_matLook;
	m_matLook.Identity();

	m_matLook = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * m_matTrans * m_matParent;

	return (_vec3)m_matLook.m[INFO_LOOK];
}

_vec3 CTransform::Get_Right()
{
	_matrix		m_matLook;
	m_matLook.Identity();

	m_matLook = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * m_matTrans * m_matParent;

	return (_vec3)m_matLook.m[INFO_RIGHT];
}

_vec3 CTransform::Get_Up()
{
	_matrix		m_matLook;
	m_matLook.Identity();

	m_matLook = m_matScale * m_matRotationX * m_matRotationY * m_matRotationZ * m_matTrans * m_matParent;

	return (_vec3)m_matLook.m[INFO_UP];
}

void CTransform::Get_LookDgreeY(_float& fDgree, _vec3 * vLook, _vec3 * vMyLook)
{
	vLook->y = 0.f;
	vMyLook->y = 0.f;

	D3DXVec3Normalize(vLook, vLook);
	D3DXVec3Normalize(vMyLook, vMyLook);

	_float fRadian = D3DXVec3Dot(vLook, vMyLook);
	_vec3 vCross = _vec3(0.f, 0.f, 0.f);
	D3DXVec3Cross(&vCross, vLook, vMyLook);

	_float fAngle = (_float)acos(fRadian);
	fDgree = D3DXToDegree(fAngle);

	if (fRadian < 0)
		fDgree = 360 - fDgree;

	if (vCross.y > 0.f)
		fDgree *= -1.f;
}

void CTransform::Set_Degree(const _vec3 & vRot)
{
	m_fDegreeX = vRot.x;
	m_fDegreeY = vRot.y;
	m_fDegreeZ = vRot.z;

	Rotation_X(0);
	Rotation_Y(0);
	Rotation_Z(0);
}

void CTransform::Scaling(const _vec3 & vScale)
{
	m_matScale._11 = vScale.x;
	m_matScale._22 = vScale.y;
	m_matScale._33 = vScale.z;
}

void CTransform::Scaling(const _float & x, const _float & y, const _float & z)
{
	m_matScale._11 = x;
	m_matScale._22 = y;
	m_matScale._33 = z;
}

void CTransform::Rotation_X(const _float & fDegree)
{
	if (abs(m_fDegreeX) > 360)
		m_fDegreeX = 0;
	m_fDegreeX += fDegree;
	D3DXMatrixRotationX(&m_matRotationX, D3DXToRadian(m_fDegreeX));
}

void CTransform::Rotation_Y(const _float & fDegree)
{
	if (abs(m_fDegreeY) > 360)
		m_fDegreeY = 0;
	m_fDegreeY += fDegree;
	D3DXMatrixRotationY(&m_matRotationY, D3DXToRadian(m_fDegreeY));
}

void CTransform::Rotation_Z(const _float & fDegree)
{
	if (abs(m_fDegreeZ) > 360)
		m_fDegreeZ = 0;
	m_fDegreeZ += fDegree;
	D3DXMatrixRotationZ(&m_matRotationZ, D3DXToRadian(m_fDegreeZ));

}

void CTransform::BillBoard_RotationX(const _float & fAngle, const _float & fTimeDelta)
{
	_vec3		vUp = _vec3(0.f, 1.f, 0.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_UP][0]);
	_vec3		vLook = _vec3(0.f, 0.f, 1.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_LOOK][0]);

	_matrix			matRot;
	D3DXMatrixRotationX(&matRot, D3DXToRadian(fAngle));
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_UP][0], &vUp, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_LOOK][0], &vLook, &matRot);
}

void CTransform::BillBoard_RotationY(const _float & fAngle, const _float & fTimeDelta)
{
	_vec3		vRight = _vec3(0.f, 1.f, 0.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_RIGHT][0]);
	_vec3		vLook = _vec3(0.f, 0.f, 1.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_LOOK][0]);

	_matrix			matRot;
	D3DXMatrixRotationY(&matRot, D3DXToRadian(fAngle));
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_RIGHT][0], &vRight, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_LOOK][0], &vLook, &matRot);
}

void CTransform::BillBoard_RotationZ(const _float & fAngle, const _float & fTimeDelta)
{
	_vec3		vUp = _vec3(0.f, 1.f, 0.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_UP][0]);
	_vec3		vRight = _vec3(1.f, 0.f, 0.f) * D3DXVec3Length((_vec3*)&m_matWorld.m[INFO_RIGHT][0]);
	
	_matrix			matRot;
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(m_fDegreeZ));
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_UP][0], &vUp, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_RIGHT][0], &vRight, &matRot);
}

void CTransform::Translation(const _vec3 & vTranslation)
{
	m_matTrans._41 = vTranslation.x;
	m_matTrans._42 = vTranslation.y;
	m_matTrans._43 = vTranslation.z;
}

void CTransform::Translation(const _float & x, const _float & y, const _float & z)
{
	m_matTrans._41 = x;
	m_matTrans._42 = y;
	m_matTrans._43 = z;
}

void CTransform::Parent(const _matrix& matParent) 
{
	m_matParent = matParent;
}


void CTransform::Move_ToDirection(const _vec3 & vDirection, const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3& vPos = (_vec3&)m_matTrans.m[INFO_POSITION];

	_vec3 vDir = vDirection;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir * fSpeed * fTimeDelta;
}

void CTransform::Go_Straight(const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3& vPos = (_vec3&)m_matTrans.m[INFO_POSITION];
	_vec3 vLook = (_vec3&)m_matWorld.m[INFO_LOOK];

	vLook.y = 0;
	D3DXVec3Normalize(&vLook, &vLook);

	vPos += vLook * fSpeed * fTimeDelta;
}

void CTransform::Go_Right(const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3& vPos = (_vec3&)m_matTrans.m[INFO_POSITION];
	_vec3 vDir = (_vec3&)m_matWorld.m[INFO_RIGHT];

	D3DXVec3Normalize(&vDir, &vDir);
	vPos += vDir * fSpeed * fTimeDelta;
}

void CTransform::Sliding(const _float & fSpeed, const _float & fTimeDelta, const _vec3 & vNormal)
{
	_vec3& vDir = (_vec3&)m_matWorld.m[INFO_LOOK];

	_float vDot = D3DXVec3Dot(&vDir, &vNormal);

	_vec3& vSlidingVec = vDir - D3DXVec3Dot(&vDir, &vNormal) * vNormal;

	D3DXVec3Normalize(&vSlidingVec, &vSlidingVec);

	_vec3& vPos = (_vec3&)m_matTrans.m[INFO_POSITION];

	vPos += vSlidingVec * fSpeed * fTimeDelta;

}

void CTransform::Get_SlidingVec(_vec3* vSliding, const _float & fSpeed, const _float & fTimeDelta, const _vec3 & vNormal)
{

	//_vec3 vPlayerGo = m_pTransformCom->Get_Position();
	//_vec3	vDirection;
	//vPlayerGo += *D3DXVec3Normalize(&vDirection, &m_pTransformCom->Get_Look()) * 0.5f/* * 2.f * fTimeDelta*/;

	_vec3& vDir = (_vec3&)m_matWorld.m[INFO_LOOK];

	_vec3& vSlidingVec = vDir - D3DXVec3Dot(&vDir, &vNormal) * vNormal;

	D3DXVec3Normalize(&vSlidingVec, &vSlidingVec);

	_vec3& vPos = (_vec3&)m_matTrans.m[INFO_POSITION];

	*vSliding = vPos + vSlidingVec * 0.5f;
}

void CTransform::Get_NextMove(_vec3 * vSliding, const _vec3* vLook)
{
	_vec3	vPlayerGo = (_vec3&)m_matTrans.m[INFO_POSITION];

	//if (vPlayerGo.x < 0)
	//	vPlayerGo.x *= -1.f;
	//if (vPlayerGo.z < 0)
	//	vPlayerGo.z *= -1.f;

	_vec3	vDirection;
	*vSliding = vPlayerGo + *D3DXVec3Normalize(&vDirection, vLook) * 0.8f;
}
