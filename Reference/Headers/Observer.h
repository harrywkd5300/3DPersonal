#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Subject_Manager.h"

BEGIN(Engine)

class DLL_EXPORT CObserver : public CBase
{
protected:
	explicit CObserver();
	virtual ~CObserver() = default;

public:
	virtual void Update(int iMessage, void* pData)PURE;
	virtual HRESULT Ready_Observer(void)PURE;

public:
	virtual _ulong Free();

protected:
	CSubject_Manager* m_pSubject_Manager = nullptr;
};

END