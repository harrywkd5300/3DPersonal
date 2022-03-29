#include "stdafx.h"
#include "..\Headers\Camera_Dynamic.h"

#include "Input_Device.h"
#include "CameraObserver.h"

CCamera_Dynamic::CCamera_Dynamic(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
{
}

HRESULT CCamera_Dynamic::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	if (FAILED(Engine::CCamera::Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCamera_Dynamic::Ready_Observer()
{
	//m_pObserver = CCameraObserver::Create();
	//CSubject_Manager::GetInstance()->AddObserver(m_pObserver);

	//m_pSubject_Manager->AddData(MESSAGE_CAMERA_DATA, this);

	return NOERROR;
}

_int CCamera_Dynamic::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pInput_Device)
		return -1; 

	if (m_bState)
	{
		Event_State();
	}
	else
	{
		Check_InputState(fTimeDelta);
	}

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CCamera_Dynamic::Check_InputState(const _float & fTimeDelta)
{
	//cout << m_vAt.x << " : " << m_vAt.y << " : " << m_vAt.z << endl;
	if (m_pInput_Device->Get_DIKeyState(DIK_UP) & 0x80)
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;		
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * -m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * -m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}
	
	_long		 Move = 0;

	_vec3		vLook;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));

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

void CCamera_Dynamic::Fix_MousePointer(const _uint & iX, const _uint & iY)
{
	POINT			pt = { LONG(iX), LONG(iY) };

	ClientToScreen(g_hWnd, &pt);

	SetCursorPos(pt.x, pt.y);

	ShowCursor(false);

}

void CCamera_Dynamic::Event_State()
{
	switch (m_pCurState)
	{
	case CCamera_Dynamic::CAM_SNOWMAN:

		break;
	default:
		break;
	}
}

CCamera_Dynamic * CCamera_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CCamera_Dynamic *	pInstance = new CCamera_Dynamic(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	{
		MSG_BOX("CCamera_Dynamic Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCamera_Dynamic::Free(void)
{
	return Engine::CCamera::Free();
}

