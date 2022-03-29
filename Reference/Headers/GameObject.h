#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class CComponent;
class DLL_EXPORT CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameObject() = default;
public:
	CComponent*	Get_Component(const _tchar* pComTag);
	void		Delete_Component(const _tchar* pComTag);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
protected:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
protected:
	map<const _tchar*, CComponent*>			m_mapComponent[Engine::CComponent::TYPE_END];
	typedef map<const _tchar*, CComponent*>	MAPCOMPONENT;

	_bool									m_bIsDead = false;
	_bool									m_bIsLastDead = false;
	_float									m_fSortingDist = 0.f;
	_bool									m_fNotUpdate = false;

private:
	CComponent* Find_Component(const _tchar* pComTag);

public:
	_bool			Get_IsDead() { return m_bIsDead; }
	void			Set_IsDead() { m_bIsDead = true; }

	_bool			Get_IsLastDead() { return m_bIsLastDead; }
	void			Set_IsLastDead() { m_bIsLastDead = true; }

	void			Set_isUpdate(_bool Update) { m_fNotUpdate = Update; }

	_float&			Get_CamDist() { return m_fSortingDist; }
protected:
	virtual _ulong Free(void) override;
};

END