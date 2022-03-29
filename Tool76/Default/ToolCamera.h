#pragma once

#include "camera.h"
#include "Tool_Define.h"
#include "Input_Device.h"

class CToolCamera final : public CCamera
{
private:
	explicit CToolCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CToolCamera() = default;

public:
	HRESULT Ready_Camera_Dynamic(const _vec3 & vEye, const _vec3 & vAt, const _vec3 & vWorldUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar);

	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	_int		 Update_InputState(const _float& fTimeDelta);

	_bool	m_MouseMove = false;
	_bool	m_NotMove = false;

	_float	m_fCamSpeed = 0.f;

	void Fix_MousePointer(const _uint & iX, const _uint & iY);
	/*MEMBER*/
private:
	/*Look At this Degree*/
	_bool	m_bMouseMode = false;
	CInput_Device*	m_pInput_Device = nullptr;

	/*related ctor dtor*/
public:
	static CToolCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 & vEye, const _vec3 & vAt, const _vec3 & vWorldUp, const _float & fFovY = D3DXToRadian(60.f), const _float & fAspect = (_float)BACKCX / (_float)BACKCY, const _float & fNear = 0.3f, const _float & fFar = 1000.f);

private:
	virtual _ulong Free() final;
};

