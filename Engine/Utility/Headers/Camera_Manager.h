#pragma once

#include "Base.h"
#include "Engine_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class DLL_EXPORT CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	CCamera*	Get_Camera(const _tchar* pCameraTag);
	CCamera*	Get_CurCamera();

	void		Set_CameraPos(const _vec3& v) { m_pCurCamera->Set_CameraEye(v); }

public:
	HRESULT		Add_Camera(const _tchar* pCameraTag, CGameObject* pGameObject);
	HRESULT		Change_Camera(const _tchar* pCameraTag);

public:
	_int Update_Camera_Manager(const _float& fTimeDelta);
	_int LastUpdate_Camera_Manager(const _float& fTimeDelta);

private:
	map<const _tchar*, CCamera*>			m_pMapCamera;
	typedef map<const _tchar*, CCamera*>	MAPCAMERA;

	CCamera*								m_pCurCamera = nullptr;

public:
	CCamera* Find_Camera(const _tchar* pLayerTag);
private:
	virtual _ulong Free(void) final;
};

END