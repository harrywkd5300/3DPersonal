#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CComponent;
class DLL_EXPORT CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;
public:
	CComponent* Get_Component(const _uint& iIndex, const _tchar* pComTag);
public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
public:
	HRESULT Ready_Layer(void);
	_int Update_Layer(const _float& fTimeDelta);
	_int LastUpdate_Layer(const _float& fTimeDelta);
	void Render_Layer(void);
public:
	void Dead_AllObject();
	list<CGameObject*>*		GetObjList() { return &m_ObjectList; }
private:
	list<CGameObject*>				m_ObjectList;
	typedef list<CGameObject*>		OBJECTLIST;


public:
	static CLayer* Create(void);
private:
	virtual _ulong Free(void) final;
};

END