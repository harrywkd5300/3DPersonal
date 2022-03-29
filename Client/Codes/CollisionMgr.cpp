#include "stdafx.h"
#include "..\Headers\CollisionMgr.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}


_bool CCollisionMgr::Check_Coll(CCollider * pDest, CCollider * pSour)
{
	pDest->Set_Coll(false);
	pSour->Set_Coll(false);

	pDest->Collision_AABB(pSour);

	if (pDest->Get_IsColl())
		return true;

	return false;
}

void CCollisionMgr::Check_Monster(list<CGameObject*>* pMonster)
{
	auto& iter     = pMonster->begin();
	auto& iter_end = pMonster->end();

	for (; iter != iter_end; ++iter)
		dynamic_cast<CMonster*>(*iter)->Set_CheckCol(false);

	iter = pMonster->begin();

	auto& iter_dest = pMonster->begin();
	auto& iter_dest_end = pMonster->end();

	for (; iter != iter_end; ++iter)
	{
		if (dynamic_cast<CMonster*>(*iter)->Get_CheckCol() == true)
			continue;

		for (; iter_dest != iter_dest_end; ++iter_dest)
		{
			if (iter == iter_dest)
				continue;
			CTransform* pTransform = (CTransform*)dynamic_cast<CMonster*>(*iter)->Get_Component(L"Com_Transform");
			CTransform* pTransform_dest = (CTransform*)dynamic_cast<CMonster*>(*iter_dest)->Get_Component(L"Com_Transform");
			_vec3 vShiedPos = pTransform_dest->Get_Position();
			_float fDist = pTransform->Compute_Distance(&vShiedPos);
			if (fDist > 4.f)
				continue;

			CCollider*		pCollider = (CCollider*)(*iter)->Get_Component(L"Com_Collider");
			CComponent*		pCollider_dest = (*iter_dest)->Get_Component(L"Com_Collider");
			if (true == pCollider->Collision_AABB((CCollider*)pCollider_dest))
			{
				dynamic_cast<CMonster*>(*iter_dest)->Set_CheckCol(true);
				dynamic_cast<CMonster*>(*iter_dest)->Set_CheckColMove(true);
				break;
			}
			//else
			//	dynamic_cast<CMonster*>(*iter_dest)->Set_CheckColMove(false);
		}
	}
}

_ulong CCollisionMgr::Free(void)
{
	return 0;
}
