#include "..\Headers\Component_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
	: m_pMapComponents(nullptr)
{
}

HRESULT CComponent_Manager::Ready_Component(const _uint& iArrayIdx, const _tchar * pComponentTag, CComponent * pComponent)
{
	if (nullptr == m_pMapComponents
		|| iArrayIdx >= m_iContainerSize)
		return E_FAIL;
	
	CComponent* pComponent_Find = Find_Component(iArrayIdx, pComponentTag);
	if (nullptr != pComponent_Find)
		return E_FAIL;

	m_pMapComponents[iArrayIdx].insert(MAPCOMPONENTS::value_type(pComponentTag, pComponent));
	
	return NOERROR;
}

CComponent* CComponent_Manager::Clone_Component(const _uint & iArrayIdx, const _tchar * pComponentTag)
{
	if (iArrayIdx >= m_iContainerSize)
		return nullptr;

	CComponent* pComponent = Find_Component(iArrayIdx, pComponentTag);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone();
}

HRESULT CComponent_Manager::Reserve_ComponentContainer(const _uint & iMaxSize)
{
	if (nullptr != m_pMapComponents)
	{
		MSG_BOX("Reserve_ResourceContainer Failed");
		return E_FAIL;
	}

	m_pMapComponents = new MAPCOMPONENTS[iMaxSize];

	m_iContainerSize = iMaxSize;

	return NOERROR;
}

HRESULT CComponent_Manager::Release_Component(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (auto& Pair : m_pMapComponents[iSceneIdx])
	{
		if (dwRefCnt = Safe_Release(Pair.second))
		{
			MSG_BOX("Scene`s Component Release Failed");
			return E_FAIL;
		}
	}
	m_pMapComponents[iSceneIdx].clear();

	return NOERROR;
}

HRESULT CComponent_Manager::Release_TargetComponent(const _uint & iSceneIdx, const _tchar * pComponentTag)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (auto& Pair : m_pMapComponents[iSceneIdx])
	{
		CComponent* pComponent = Find_Component(iSceneIdx, pComponentTag);
		if (nullptr == pComponent)
			return E_FAIL;

		if (Pair.second == pComponent)
		{
			if (dwRefCnt = Safe_Release(Pair.second))
			{
				MSG_BOX("Scene`s TargetComponent Release Failed");
				return E_FAIL;
			}
			m_pMapComponents[iSceneIdx].erase(Pair.first);
			return NOERROR;
		}
	}
	return NOERROR;
}

CComponent * CComponent_Manager::Find_Component(const _uint& iArrayIdx, const _tchar * pComponentTag)
{
	if (nullptr == m_pMapComponents
		|| iArrayIdx >= m_iContainerSize)
		return nullptr;

	auto iter = find_if(m_pMapComponents[iArrayIdx].begin(), m_pMapComponents[iArrayIdx].end(), CFind_Tag(pComponentTag));

	if(iter == m_pMapComponents[iArrayIdx].end())		
		return nullptr;

	return iter->second;
}

_ulong CComponent_Manager::Free(void)	//By.Jang
{
	for (size_t i = 0; i < m_iContainerSize; i++)
	{
		for_each(m_pMapComponents[i].begin(), m_pMapComponents[i].end(), CRelease_Pair());
		m_pMapComponents[i].clear();
	}
	Safe_Delete_Array(m_pMapComponents);
	return 0;
}
