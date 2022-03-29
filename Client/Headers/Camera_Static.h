#pragma once

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CTransform;
	class CInput_Device;
}

class CCamera_Static final : public Engine::CCamera
{
public:
	enum CAMERA_STATE { STATE_BASE, STATE_BULID, STATE_REBULID, STATE_END };
private:
	explicit CCamera_Static(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera_Static(void) = default;
public:
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
private:

	void Fix_MousePointer(const _uint& iX, const _uint& iY);
	void State_Camera(const _float& fTimeDelta);
	void Key_Input(const _float& fTimeDelta);
private:
	_float				m_fCamSpeed = 5.f;
	_float				m_fDir = 5.f;
	_vec3				m_PlayerPos;

	CTransform*			m_pTargetCom = nullptr;
	CInput_Device*		m_pInput_Device = nullptr;


	_bool				m_bFirst = true;
	_bool				m_bBuildMove = true;
public:
	CAMERA_STATE		m_eCurStateType;

public:   // Setter
	void Set_PlayerPos(_vec3 vPlayerPos) { m_PlayerPos = vPlayerPos; }
	void Set_CameraCurState(CAMERA_STATE state) { m_eCurStateType = state; }
	void Set_Target();
	
public:   // Getter
	_bool	Get_Build() { return m_bBuildMove; }



	CAMERA_STATE Get_CurState() { return m_eCurStateType; }
public:
	static CCamera_Static* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY = D3DXToRadian(60.0f), const _float& fAspect = _float(BACKSIZEX) / _float(BACKSIZEY), const _float& fNear = 0.2f, const _float& fFar = 1000.f);
private:
	virtual _ulong Free(void);

};
