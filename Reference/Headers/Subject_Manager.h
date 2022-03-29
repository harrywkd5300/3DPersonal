#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CObserver;


class DLL_EXPORT CSubject_Manager : public CBase
{
	DECLARE_SINGLETON(CSubject_Manager)
private:
	explicit CSubject_Manager();
	virtual ~CSubject_Manager() = default;

public:
	list<void*>*	GetDataList(int iMessage); // ����� ����Ʈ Get�Լ�

public: // Observer
	void AddObserver(CObserver* pObserver); // ������Ʈ ���� , Static �� Dynamic ���� 
	void RemoveObserver(CObserver* pObserver); // ������Ʈ�� ����Ʈ���� ����
	void ReleaseObserver(); 
	void Notify(int iMessage, void* pData = NULL); // �������� ������ ���ߴٸ� ȣ���� �Լ�.

public: // Data
	void AddData(int iMessage,  void* pData); // ������ ����
	void RemoveData(int iMessage,void* pData); // ������ �����

	void ReleaseData(); // Type�� �´� ����� ������ ���� �����

	void Delete_DynamicData(); // Dynamic Data ���� �����
private:
	map<_int, list<void*>> m_mapData; // Data�� ������ ����Ʈ���� // -> List�� ����ִµ� ���� ����غ��� �� �Ͱ���.
	typedef map<_int, list<void*>> MAPSUBJECT;

	list<CObserver*>	m_ListObserver; // �������� ������ ����Ʈ����
	typedef list<CObserver*> OBSERVERLIST;

public:
	virtual _ulong Free(); // ���������� �����͸� �����Ҵ�� �޸𸮸� ������������


};

END

