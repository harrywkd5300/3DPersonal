#pragma once


#include "Base.h"
#include "Defines.h"

#include "Collider.h"
#include "Monster.h"

class CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr()= default;

public:
	_bool	Check_Coll(CCollider* pDest, CCollider* pSour);
	void	Check_Monster(list<CGameObject*>* pMonster);

private:
	virtual _ulong Free(void) final;
};
