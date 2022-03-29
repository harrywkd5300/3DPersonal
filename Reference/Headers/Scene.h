#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"

BEGIN(Engine)


class DLL_EXPORT CScene abstract : public CBase
{	
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene() = default;
public:
	virtual HRESULT Ready_Scene(void);
	virtual _int Update_Scene(const _float& fTimeDelta);
	virtual _int LastUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void);
public:
	HRESULT Release_Object(const _uint& iSceneIdx);
protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
protected:
	Engine::CObject_Manager*		m_pObject_Manager = nullptr;
	Engine::CComponent_Manager*		m_pComponent_Manager = nullptr;
	Engine::CCamera_Manager*		m_pCamera_Manager = nullptr;
protected:
	virtual _ulong Free(void) override;
};

END