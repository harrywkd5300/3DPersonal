#pragma once


#include "GameObject.h"

// Client상에 필요한 뷰스페이스 변환 and 투영 변환에대한 관리를 수행한다.

BEGIN(Engine)

class CInput_Device;
class DLL_EXPORT CCamera abstract : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera(void) = default;

	/*FUNCTION*/
public:
	const _vec3& Get_CameraPos() {
		return (_vec3&)m_matWorld.m[3];
	};
	const _matrix Get_CameraWorldMatrix() {
		return m_matWorld;
	}
	const _vec3& Get_CameraEye() { return m_vEye; }

	void Set_CameraEye(const _vec3& View) { m_vEye = View; }

public:
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
									, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
protected:
	CInput_Device*		m_pInput_Device = nullptr;
protected:
	_matrix				m_matWorld;
protected:	
	_matrix				m_matView;
	_vec3				m_vEye;
	_vec3				m_vAt;
	_vec3				m_vFixedUp;
protected:
	_matrix				m_matProj;
	_float				m_fFovY;
	_float				m_fAspect;
	_float				m_fNear;
	_float				m_fFar;
private:
	void SetUp_ViewMatrix();
	void SetUp_ProjMatrix();
protected:
	virtual _ulong Free(void);
};

END