#include "stdafx.h"
#include "..\Headers\Camera_Static.h"

#include "Component_manager.h"
#include "Object_Manager.h"
#include "layer.h"
#include "Player.h"

#include "input_Device.h"

CCamera_Static::CCamera_Static(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_pTargetCom(nullptr)
	, m_pInput_Device(CInput_Device::GetInstance()), m_bBuildMove(true)
{
	m_pInput_Device->AddRef();
}

HRESULT CCamera_Static::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	if (FAILED(Engine::CCamera::Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		return E_FAIL;

	m_eCurStateType = STATE_BASE;

	return NOERROR;
}

_int CCamera_Static::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pTargetCom)
	{
		m_pTargetCom = (CTransform*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STATIC, L"Layer_Player", 0, L"Com_Transform");
		if(nullptr != m_pTargetCom)
			m_pTargetCom->AddRef();
	}
	if (nullptr != m_pTargetCom)
	{
		Key_Input(fTimeDelta);

		State_Camera(fTimeDelta);
	}
	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

_int CCamera_Static::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return 0;
}


void CCamera_Static::Fix_MousePointer(const _uint & iX, const _uint & iY)
{
	POINT			pt = { LONG(iX), LONG(iY) };

	ClientToScreen(g_hWnd, &pt);

	SetCursorPos(pt.x, pt.y);

	ShowCursor(TRUE);

}

void CCamera_Static::Key_Input(const _float & fTimeDelta)
{
}

void CCamera_Static::State_Camera(const _float & fTimeDelta)
{
	switch (m_eCurStateType)
	{
	case CCamera_Static::STATE_BASE:
	{
		(BACKSIZEX >> 1, BACKSIZEY >> 1);

		m_bBuildMove = true;

		_vec3 vPlayerDir = m_pTargetCom->Get_Look();
		_vec3 vPlayerPos = m_pTargetCom->Get_Position();

		_vec3 vPlayerRight = m_pTargetCom->Get_Right();

		D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);
		_vec3 vLook = vPlayerPos;
		vLook.z += 0.3f;
		vLook.y += 1.2f;
		m_vAt = vLook;

		_vec3 vPos = vPlayerPos - vPlayerDir * 1.9f;
		vPos.y += 1.2f;
		vPos.z += 0.3f;
		m_vEye = vPos;

		break;
	}
	case CCamera_Static::STATE_BULID:
	{
		//_vec3 vPlayerDir = m_pTargetCom->Get_Up();
		//_vec3 vPlayerPos = m_pTargetCom->Get_Position();
		//_vec3 vRight = m_pTargetCom->Get_Right();
		//_vec3		vPos;
		//_matrix		matRot;
		//D3DXMatrixRotationAxis(&matRot, &vPlayerPos, D3DXToRadian(-90.f * fTimeDelta));
		//D3DXVec3TransformNormal(&vPos, &m_vEye, &matRot);
		//D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);
		//_vec3 vLook = vPlayerPos;
		//m_vAt = vLook;
		////_vec3 vPos = vPlayerPos + vPlayerDir * 2.f;
		//m_vEye = vPos;

		m_bBuildMove = false;

		_vec3 vPlayerDir = m_pTargetCom->Get_Up();
		_vec3 vPlayerPos = m_pTargetCom->Get_Position();
		_vec3 vPlayerLook = m_pTargetCom->Get_Look();

		D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);
		D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

		_vec3 vLook = vPlayerPos;

		m_vAt = vLook;
		m_vAt.z += 0.001f;

		_vec3 vPos = vPlayerPos + vPlayerDir * 3.f - vPlayerLook * 0.2f;

		_vec3 vDir = vPos - m_vEye;
		_float fDist = D3DXVec3Length(&vDir);
		if (fDist > 0.1f)
			m_vEye += *D3DXVec3Normalize(&vDir, &vDir) * 2.f * fTimeDelta;
		else
		{
			m_bBuildMove = true;
			m_vEye = vPos;
		}

		break;
	}
	case CCamera_Static::STATE_REBULID:
	{
		m_bBuildMove = false;

		_vec3 vPlayerDir = m_pTargetCom->Get_Look();
		_vec3 vPlayerPos = m_pTargetCom->Get_Position();
		D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);
		_vec3 vLook = vPlayerPos;
		vLook.z += 0.2f;
		vLook.y += 1.4f;
		m_vAt = vLook;

		_vec3 vPos = vPlayerPos - vPlayerDir * 1.9f;
		vPos.y += 1.4f;
		vPos.z += 0.2f;
		//_vec3 vLook = vPlayerPos;
		//vLook.x += 0.1f;
		//vLook.y += 0.8f;
		//m_vAt = vLook;

		//_vec3 vPos = vPlayerPos - vPlayerDir * 1.2f;
		//vPos.x += 0.1f;
		//vPos.y += 0.8f;

		_vec3 vDir = vPos - m_vEye;

		_float fDist = D3DXVec3Length(&vDir);

		if (fDist > 0.05f)
			m_vEye += *D3DXVec3Normalize(&vDir, &vDir) * 2.f * fTimeDelta;
		else
		{
			m_bBuildMove = true;
			auto& iter = CObject_Manager::GetInstance()->Find_Layer(SCENE_STATIC, L"Layer_Player")->GetObjList()->front();
			dynamic_cast<CPlayer*>(iter)->Set_EndBuild();
			m_eCurStateType = STATE_BASE;
		}
		break;
	}
	case CCamera_Static::STATE_END:
		break;
	default:
		break;
	}
}

void CCamera_Static::Set_Target()
{
	Safe_Release(m_pTargetCom);
}

CCamera_Static * CCamera_Static::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CCamera_Static *	pInstance = new CCamera_Static(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	{
		MSG_BOX("CCamera_Dynamic Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCamera_Static::Free(void)
{

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTargetCom);
	
	return Engine::CCamera::Free();
}
