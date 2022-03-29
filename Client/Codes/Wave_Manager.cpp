#include "stdafx.h"
#include "..\Headers\Wave_Manager.h"

IMPLEMENT_SINGLETON(CWave_Manager)

CWave_Manager::CWave_Manager()
{

}

HRESULT CWave_Manager::Add_WaveMonster(POINT ePoint, CGameObject* pObject)
{
	if (nullptr == pObject)
		return E_FAIL;

	m_WaveList.push_back(pObject);

	return NOERROR;
}

void CWave_Manager::MonsterDead(CGameObject * pObject)
{
	auto& iter = m_WaveList.begin();
	auto& iter_end = m_WaveList.end();
	_uint j = 0;
	for (; iter != iter_end; ++iter)
	{
		if ((*iter) == pObject)
		{
			m_WaveList.erase(iter);
			return;
		}
	}
}

void CWave_Manager::Clear_Wave()
{
	auto& iter = m_WaveList.begin();
	auto& iter_end = m_WaveList.end();
	for (; iter != iter_end; ++iter)
		(*iter)->Set_IsDead();
	m_WaveList.clear();
}

_ulong CWave_Manager::Free(void)
{
	return 0;
}
