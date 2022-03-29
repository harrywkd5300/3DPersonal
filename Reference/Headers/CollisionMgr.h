#pragma once


#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CCollider;
class DLL_EXPORT CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr()= default;

public:
	_bool	Check_Coll(CCollider* pDest, CCollider* pSour);

private:
	virtual _ulong Free(void) final;
};

END