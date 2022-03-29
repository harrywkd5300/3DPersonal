#pragma once

// For.Ÿ�̸ӵ��� ������ �����ϱ����� ������� Ŭ����
#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class DLL_EXPORT CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
private:
	explicit CTimer_Manager(void);
	virtual ~CTimer_Manager(void);
public:
	float Get_TimeDelta(const TCHAR* pTimerTag) const;

public: //�߰��ϰڴ�.
	HRESULT Add_Timer(const TCHAR* pTimerTag, CTimer* pTimer);
	void Compute_Timer(const TCHAR* pTimerTag);
private:
	map<const TCHAR*, CTimer*>			m_mapTimer;
public:
	typedef map<const TCHAR*, CTimer*>	MAPTIMER;
private:
	CTimer* Find_Timer(const TCHAR* pTimerTag) const;
private:
	virtual _ulong Free(void) final;
};

END