#include "stdafx.h"
#include "ToolCamera.h"


CToolCamera::CToolCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
	, m_bMouseMode(false)
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_fCamSpeed(0.f)
	, m_NotMove(false)
{
	m_pInput_Device->AddRef();
}

HRESULT CToolCamera::Ready_Camera_Dynamic(const _vec3& vEye, const _vec3& vAt, const _vec3& vWorldUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float & fFar)
{
	if (FAILED(CCamera::Ready_GameObject(&vEye, &vAt, &vWorldUp, fFovY, fAspect, fNear, fFar)))
		return E_FAIL;

	m_fCamSpeed = 9.f;

	return NOERROR;
}

_int CToolCamera::Update_GameObject(const _float & fTimeDelta)
{
	if (m_MouseMove)
	{
		Fix_MousePointer((BACKCX + BACKCX / 5) >> 1, BACKCY >> 1);
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_F1) & 0x80)
	{
		if (m_NotMove)
			m_NotMove = false;
		else
			m_NotMove = true;
	}


	Update_InputState(fTimeDelta);

	CCamera::Update_GameObject(fTimeDelta);

	return 0;
}

_int CToolCamera::LastUpdate_GameObject(const _float & fTimeDelta)
{

	CCamera::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

_int CToolCamera::Update_InputState(const _float & fTimeDelta)
{
	_vec3		vLook, vUp, vRight;
	memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));
	memcpy(&vUp,    &m_matWorld.m[1][0], sizeof(_vec3));
	memcpy(&vLook,  &m_matWorld.m[2][0], sizeof(_vec3));

	if (m_NotMove)
	{
		if (m_pInput_Device->Get_DIKeyState(DIK_W) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3 vMove = *D3DXVec3Normalize(&vUp, &vUp) * m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}

		if (m_pInput_Device->Get_DIKeyState(DIK_S) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3	vMove = *D3DXVec3Normalize(&vUp, &vUp) * -m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}

		if (m_pInput_Device->Get_DIKeyState(DIK_A) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * -m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}
		if (m_pInput_Device->Get_DIKeyState(DIK_D) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}

		if (m_pInput_Device->Get_DIKeyState(DIK_Q) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}

		if (m_pInput_Device->Get_DIKeyState(DIK_E) & 0x80)
		{
			// 카메라의 월드행렬
			_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * -m_fCamSpeed * fTimeDelta;
			m_vEye += vMove;
			m_vAt += vMove;
		}

		_long		 Move = 0;
		if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_Y))
		{
			_matrix			matRot;
			_vec3			vRight;
			memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));
			D3DXMatrixRotationAxis(&matRot, &vRight, Move * 0.1f * fTimeDelta);
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
			m_vAt = m_vEye + vLook;
		}

		if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_X))
		{
			_matrix			matRot;
			_vec3			vUp(0.f, 1.f, 0.f);
			memcpy(&vUp, &m_matWorld.m[1][0], sizeof(_vec3));
			D3DXMatrixRotationAxis(&matRot, &vUp, Move * 0.1f * fTimeDelta);
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
			m_vAt = m_vEye + vLook;
		}
	}

	// Camera Mouse Move
	if (m_pInput_Device->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		if (!m_MouseMove)
			m_MouseMove = true;
		else
			m_MouseMove = false;
	}

	return 0;
}

void CToolCamera::Fix_MousePointer(const _uint & iX, const _uint & iY)
{
	POINT			pt = { LONG(iX), LONG(iY) };

	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = pWnd->m_hWnd;

	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CToolCamera * CToolCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 & vEye, const _vec3 & vAt, const _vec3 & vWorldUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CToolCamera* pInstance = new CToolCamera(pGraphicDev);

	if (pInstance->Ready_Camera_Dynamic(vEye, vAt, vWorldUp, fFovY, fAspect, fNear, fFar))
	{
		MSG_BOX("CToolCamera creation failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CToolCamera::Free()
{
	/*decrease refCount*/
	Safe_Release(m_pInput_Device);

	return CCamera::Free();
}
