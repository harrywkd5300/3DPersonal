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
	list<void*>*	GetDataList(int iMessage); // 저장된 리스트 Get함수

public: // Observer
	void AddObserver(CObserver* pObserver); // 오브젝트 저장 , Static 및 Dynamic 설정 
	void RemoveObserver(CObserver* pObserver); // 오브젝트를 리스트에서 삭제
	void ReleaseObserver(); 
	void Notify(int iMessage, void* pData = NULL); // 데이터의 정보가 변했다면 호출할 함수.

public: // Data
	void AddData(int iMessage,  void* pData); // 데이터 삽입
	void RemoveData(int iMessage,void* pData); // 데이터 지우기

	void ReleaseData(); // Type에 맞는 저장된 데이터 전부 지우기

	void Delete_DynamicData(); // Dynamic Data 전부 지우기
private:
	map<_int, list<void*>> m_mapData; // Data를 보관할 리스트변수 // -> List로 담겨있는데 좀더 고민해봐야 할 것같다.
	typedef map<_int, list<void*>> MAPSUBJECT;

	list<CObserver*>	m_ListObserver; // 옵저버를 보관할 리스트변수
	typedef list<CObserver*> OBSERVERLIST;

public:
	virtual _ulong Free(); // 프리에서는 데이터를 동적할당된 메모리를 해제하지않음


};

END

