#include "..\Headers\Subject_Manager.h"

#include "Observer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CSubject_Manager)


CSubject_Manager::CSubject_Manager()
{
}

list<void*>* CSubject_Manager::GetDataList(int iMessage)
{
	MAPSUBJECT::iterator iter;

	iter = m_mapData.find(iMessage);

	if (iter == m_mapData.end())
		return NULL;

	return &iter->second;
}

void CSubject_Manager::AddObserver(CObserver * pObserver)
{
	m_ListObserver.push_back(pObserver);
}

void CSubject_Manager::RemoveObserver(CObserver * pObserver)
{
	OBSERVERLIST::iterator iter = m_ListObserver.begin();
	OBSERVERLIST::iterator iter_end = m_ListObserver.end();

	for (iter; iter != iter_end; )
	{
		if ((*iter) == pObserver)
		{
			iter = m_ListObserver.erase(iter);
		}
		else
			++iter;
	}
}

void CSubject_Manager::ReleaseObserver()
{
	OBSERVERLIST::iterator iter = m_ListObserver.begin();
	OBSERVERLIST::iterator iter_end = m_ListObserver.end();

	for (; iter != iter_end; )
	{
		iter = m_ListObserver.erase(iter);
	}
}

void CSubject_Manager::Notify(int iMessage, void * pData)
{
	OBSERVERLIST::iterator iter = m_ListObserver.begin();
	OBSERVERLIST::iterator iter_end = m_ListObserver.end();

	for (iter; iter != iter_end; ++iter)
	{
		(*iter)->Update(iMessage, pData);
	}
}

void CSubject_Manager::AddData(int iMessage, void * pData)
{
	if (pData != NULL)
	{
		MAPSUBJECT::iterator iter;

		iter = m_mapData.find(iMessage);

		if (iter == m_mapData.end())
			m_mapData.insert(make_pair(iMessage, list<void*>()));

		m_mapData[iMessage].push_back(pData);
		Notify(iMessage, pData);
	}
}

void CSubject_Manager::RemoveData(int iMessage, void * pData)
{
	MAPSUBJECT::iterator iter;

	iter = m_mapData.find(iMessage);

	if (iter != m_mapData.end())
	{
		list<void*>& ListData = m_mapData[iMessage];

		for (list<void*>::iterator ListIter = ListData.begin();
			ListIter != ListData.end(); )
		{
			if ((*ListIter) == pData)
			{
				ListIter = ListData.erase(ListIter);
			}
			else
				++ListIter;
		}
	}
}

void CSubject_Manager::ReleaseData()
{
	MAPSUBJECT::iterator iter = m_mapData.begin();
	MAPSUBJECT::iterator iter_end = m_mapData.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second.clear();
	}
	m_mapData.clear();
}

void CSubject_Manager::Delete_DynamicData()
{
	MAPSUBJECT::iterator iter = m_mapData.begin();
	MAPSUBJECT::iterator iter_end = m_mapData.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second.clear();
	}
	m_mapData.clear();
}

_ulong CSubject_Manager::Free()
{
	m_ListObserver.clear();

	MAPSUBJECT::iterator iter = m_mapData.begin();
	MAPSUBJECT::iterator iter_end = m_mapData.end();

	for (iter; iter != iter_end; ++iter)
	{
		iter->second.clear();
	}
	m_mapData.clear();

	return 0;
}
