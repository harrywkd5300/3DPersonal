#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class DLL_EXPORT CComponent abstract : public CBase
{
public:
	enum COM_TYPE {TYPE_STATIC, TYPE_DYNAMIC, TYPE_END};
protected:	
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();
public:
	virtual void Update_Component(const _float& fTimeDelta) { }
public:
	virtual CComponent* Clone(void) = 0;
protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
protected:
	_bool						m_isClone = false;
protected:
	virtual _ulong Free(void) override;

};

END