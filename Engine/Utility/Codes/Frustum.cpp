#include "..\Headers\Frustum.h"

USING(Engine)


CFrustum::CFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{

}

CFrustum::CFrustum(const CFrustum & rhs)
	: CComponent(rhs)
{

}

HRESULT CFrustum::Ready_Frustum(void)
{
	SetUp_PointsInProjection();

	return NOERROR;
}

_bool CFrustum::isIn_Frustum(const _vec3 * pPositionInWorldSpace, _float fRadius)
{
	if (nullptr == m_pGraphicDev)
		return false;

	SetUp_PointsInProjection();

	_matrix		matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);

	for( size_t i = 0 ; i<8 ; ++i )
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matProj);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	for (size_t i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matView);

	D3DXPLANE			Plane[6];

	// +x, -x
	D3DXPlaneFromPoints(&Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	D3DXPlaneFromPoints(&Plane[1], &m_vPoint[4], &m_vPoint[0], &m_vPoint[3]);

	// +y, -y
	D3DXPlaneFromPoints(&Plane[2], &m_vPoint[4], &m_vPoint[5], &m_vPoint[1]);
	D3DXPlaneFromPoints(&Plane[3], &m_vPoint[3], &m_vPoint[2], &m_vPoint[6]);

	// +z, -z
	D3DXPlaneFromPoints(&Plane[4], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	D3DXPlaneFromPoints(&Plane[5], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);

	for (size_t i = 0; i < 6; ++i)
	{
		if (fRadius < D3DXPlaneDotCoord(&Plane[i], pPositionInWorldSpace))
			return false;
	}
	return true;
}

void CFrustum::SetUp_PointsInProjection(void)
{
	m_vPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _vec3( 1.f, 1.f, 0.f);
	m_vPoint[2] = _vec3( 1.f, -1.f, 0.f);
	m_vPoint[3] = _vec3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_vPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_vPoint[7] = _vec3(-1.f, -1.f, 1.f);

}

CFrustum * CFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFrustum *		pInstance = new CFrustum(pGraphicDev);

	if (FAILED(pInstance->Ready_Frustum()))
	{
		MSG_BOX("CFrustum Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CFrustum::Clone(void)
{
	return new CFrustum(*this);
}

_ulong CFrustum::Free(void)
{
	return CComponent::Free();
}
