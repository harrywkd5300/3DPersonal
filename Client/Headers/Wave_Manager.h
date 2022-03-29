#pragma once

#include "Base.h"
#include "Defines.h"

#include "GameObject.h"

class CWave_Manager : public CBase
{
	DECLARE_SINGLETON(CWave_Manager)
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum STAGE_ROUND { STAGE_ONE, STAGE_TWO, STAGE_THREE, STAGE_END };
private:
	explicit CWave_Manager();
	virtual ~CWave_Manager() = default;

public:
	HRESULT	Add_WaveMonster(POINT ePoint, CGameObject* pObject);

	void	MonsterDead(CGameObject* pObject);
	void	Clear_Wave();

	list<CGameObject*>*	Get_WaveList() { return  &m_WaveList; }

private:
	list<CGameObject*>	m_WaveList;

private:
	virtual _ulong Free(void) final;
};

