#include "..\Headers\Picking.h"
#include "VIBuffer.h"
#include "Mesh.h"

USING(Engine)

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}

CPicking::CPicking(const CPicking & rhs)
	: CComponent(rhs)
{
}

HRESULT CPicking::Ready_Picking()
{
	return NOERROR;
}

_vec3 * CPicking::Picking_ToBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix/* = nullptr*/)
{
	if (nullptr == m_pGraphicDev)
		return nullptr;

	POINT ptMouse;
	// Screen
	GetCursorPos(&ptMouse);
	// Client
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vMousePos;

	vMousePos.x = ptMouse.x / (iBackSizeX * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (iBackSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	
	//Projection Change, In.View Space
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// Ray Info
	_vec3			vRayPos, vRayDir;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// View Change, In,World Space
	_matrix			matWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matWorld);
	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	if (nullptr != pWorldMatrix)
	{
		_matrix		matWorld;
		D3DXMatrixInverse(&matWorld, nullptr, pWorldMatrix);
		D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);
	}

	const _vec3*	pVertexPos = pTargetBuffer->Get_VertexPos();
	tuple<_uint, _uint, _float>	Tuple = pTargetBuffer->Get_VertexCntXZItv();

	_float		fU = 0.f, fV = 0.f, fDist = 0.f;

	for (size_t i = 0; i < get<1>(Tuple) - 1; i++)
	{
		for (size_t j = 0; j < get<0>(Tuple) - 1; j++)
		{
			_uint		iIndex = i * get<0>(Tuple) + j;

			_uint		iLT, iRT, iLB, iRB;

			iLT = iIndex + get<0>(Tuple);
			iRT = iLT + 1;
			iLB = iIndex;
			iRB = iIndex + 1;

			// right top
			if (TRUE == D3DXIntersectTri(&pVertexPos[iRT], &pVertexPos[iLT], &pVertexPos[iRB], &vRayPos, &vRayDir
				, &fU, &fV, &fDist))
			{
				vOutDirection = (pVertexPos[iRT] + (pVertexPos[iLT] - pVertexPos[iRT]) * fU + (pVertexPos[iRB] - pVertexPos[iRT]) * fV);
				return &vOutDirection;
			}

			// left bottomK
			if (TRUE == D3DXIntersectTri(&pVertexPos[iLB], &pVertexPos[iRB], &pVertexPos[iLT], &vRayPos, &vRayDir
				, &fU, &fV, &fDist))
			{
				vOutDirection = (pVertexPos[iLB] + (pVertexPos[iRB] - pVertexPos[iLB]) * fU + (pVertexPos[iLT] - pVertexPos[iLB]) * fV);
				return &vOutDirection;
			}

		}
	}


	return nullptr;
}

_vec3 CPicking::Picking_ToNaviBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, Engine::CVIBuffer * pTargetBuffer, _ulong uX, _ulong uY, const _matrix * pWorldMatrix)
{
	if (nullptr == m_pGraphicDev)
		return _vec3(0.f, 0.f, 0.f);

	_vec3 vMousePos = _vec3(0.f, 0.f, 0.f);

	vMousePos.x =  uX / (iBackSizeX * 0.5f) - 1.f;
	vMousePos.y =  ((_float)(uY) / -(_float)(iBackSizeY * 0.5f)) + 1.f;
	vMousePos.z = 0.f;

	//Projection Change, In.View Space
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// Ray Info
	_vec3			vRayPos, vRayDir;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// View Change, In,World Space
	_matrix			matWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matWorld);
	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	if (nullptr != pWorldMatrix)
	{
		_matrix		matWorld;
		D3DXMatrixInverse(&matWorld, nullptr, pWorldMatrix);
		D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);
	}

	const _vec3*	pVertexPos = pTargetBuffer->Get_VertexPos();
	tuple<_uint, _uint, _float>	Tuple = pTargetBuffer->Get_VertexCntXZItv();

	_float		fU = 0.f, fV = 0.f, fDist = 0.f;


	for (size_t i = 0; i < get<1>(Tuple) - 1; i++)
	{
		for (size_t j = 0; j < get<0>(Tuple) - 1; j++)
		{
			_uint		iIndex = i * get<0>(Tuple) + j;

			_uint		iLT, iRT, iLB, iRB;

			iLT = iIndex + get<0>(Tuple);
			iRT = iLT + 1;
			iLB = iIndex;
			iRB = iIndex + 1;

			// right top
			if (TRUE == D3DXIntersectTri(&pVertexPos[iRT], &pVertexPos[iLT], &pVertexPos[iRB], &vRayPos, &vRayDir
				, &fU, &fV, &fDist))
			{
				vOutDirection = (pVertexPos[iRT] + (pVertexPos[iLT] - pVertexPos[iRT]) * fU + (pVertexPos[iRB] - pVertexPos[iRT]) * fV);
				return vOutDirection;
			}

			// left bottomK
			if (TRUE == D3DXIntersectTri(&pVertexPos[iLB], &pVertexPos[iRB], &pVertexPos[iLT], &vRayPos, &vRayDir
				, &fU, &fV, &fDist))
			{
				vOutDirection = (pVertexPos[iLB] + (pVertexPos[iRB] - pVertexPos[iLB]) * fU + (pVertexPos[iLT] - pVertexPos[iLB]) * fV);
				return vOutDirection;
			}

		}
	}

	return _vec3(-1.f, -1.f, -1.f);
}

_bool CPicking::Picking_ToMeshBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, LPD3DXMESH* pTargetBuffer, _ulong uX, _ulong uY, const _matrix * pWorldMatrix)
{
	if (nullptr == m_pGraphicDev)
		return false;

	_vec3 vMousePos = _vec3(0.f, 0.f, 0.f);

	vMousePos.x = uX / (iBackSizeX * 0.5f) - 1.f;
	vMousePos.y = ((_float)(uY) / -(_float)(iBackSizeY * 0.5f)) + 1.f;
	vMousePos.z = 0.f;

	//Projection Change, In.View Space
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// Ray Info
	_vec3			vRayPos, vRayDir;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// View Change, In,World Space
	_matrix			matWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matWorld);
	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	if (nullptr != pWorldMatrix)
	{
		_matrix		matWorld;
		D3DXMatrixInverse(&matWorld, nullptr, pWorldMatrix);
		D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);
	}

	_float fU = 0.f;
	_float fV = 0.f;
	_float fDist = 0.f;
	BOOL Pick_Mesh = false;


	D3DXIntersect(*pTargetBuffer, &vRayPos, &vRayDir, &Pick_Mesh, nullptr, &fU, &fV, &fDist, nullptr, nullptr);

	if (Pick_Mesh)
		return true;
	else
		return false;

	//const _vec3*	pVertexPos = pTargetBuffer->Get_VertexPos();
	//tuple<_uint, _uint, _float>	Tuple = pTargetBuffer->Get_VertexCntXZItv()
	//_float		fU = 0.f, fV = 0.f, fDist = 0.f;
	//for (size_t i = 0; i < get<1>(Tuple) - 1; i++)
	//{
	//	for (size_t j = 0; j < get<0>(Tuple) - 1; j++)
	//	{
	//		_uint		iIndex = i * get<0>(Tuple) + j;
	//		_uint		iLT, iRT, iLB, iRB;
	//		iLT = iIndex + get<0>(Tuple);
	//		iRT = iLT + 1;
	//		iLB = iIndex;
	//		iRB = iIndex + 1;
	//		// right top
	//		if (TRUE == D3DXIntersectTri(&pVertexPos[iRT], &pVertexPos[iLT], &pVertexPos[iRB], &vRayPos, &vRayDir
	//			, &fU, &fV, &fDist))
	//		{
	//			vOutDirection = (pVertexPos[iRT] + (pVertexPos[iLT] - pVertexPos[iRT]) * fU + (pVertexPos[iRB] - pVertexPos[iRT]) * fV);
	//			return vOutDirection;
	//		}
	//		// left bottomK
	//		if (TRUE == D3DXIntersectTri(&pVertexPos[iLB], &pVertexPos[iRB], &pVertexPos[iLT], &vRayPos, &vRayDir
	//			, &fU, &fV, &fDist))
	//		{
	//			vOutDirection = (pVertexPos[iLB] + (pVertexPos[iRB] - pVertexPos[iLB]) * fU + (pVertexPos[iLT] - pVertexPos[iLB]) * fV);
	//			return vOutDirection;
	//		}
	//	}
	//}
}

CComponent * CPicking::Clone(void)
{
	return new CPicking(*this);
}

CPicking * CPicking::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPicking* pInstance = new CPicking(pGraphicDev);

	if (FAILED(pInstance->Ready_Picking()))
	{
		MSG_BOX("CPicking Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CPicking::Free()
{
	return CComponent::Free();
}
