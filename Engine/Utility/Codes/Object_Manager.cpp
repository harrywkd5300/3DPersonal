#include "..\Headers\Object_Manager.h"
#include "Layer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}


CObject_Manager::~CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iSceneIdx, const _tchar * pLayerTag, const _uint & iIndex, const _tchar * pComTag)
{
	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(iIndex, pComTag);
}

HRESULT CObject_Manager::Reserve_ObjectContainer(const _uint & iMaxSize)
{
	if (nullptr != m_pMapLayer)
	{
		MSG_BOX("Reserve_ResourceContainer Failed");
		return E_FAIL;
	}

	m_pMapLayer = new MAPLAYER[iMaxSize];

	m_iContainerSize = iMaxSize;

	return NOERROR;
}

HRESULT CObject_Manager::Release_Object(const _uint & iSceneIdx)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	_ulong		dwRefCnt = 0;
	for (auto& Pair : m_pMapLayer[iSceneIdx])
	{
		if (dwRefCnt = Safe_Release(Pair.second))
		{
			MSG_BOX("Scene`s Layers Release Failed");
			return E_FAIL;
		}
	}

	m_pMapLayer[iSceneIdx].clear();

	return NOERROR;
}

HRESULT CObject_Manager::Release_TargetLayer(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	if (iSceneIdx >= m_iContainerSize)
		return E_FAIL;

	for (auto& Pair : m_pMapLayer[iSceneIdx])
	{
		CLayer* pLayer = Find_Layer(iSceneIdx, pLayerTag);
		if (nullptr == pLayer)
			return E_FAIL;

		if (Pair.second == pLayer)
		{
			Pair.second->Dead_AllObject();
			return NOERROR;
		}
	}
	return NOERROR;
}

HRESULT CObject_Manager::Ready_GameObject(const _uint & iArrayIdx, const _tchar* pLayerTag, CGameObject * pGameObject)
{
	if (nullptr == m_pMapLayer
		|| iArrayIdx >= m_iContainerSize)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iArrayIdx, pLayerTag);

	if (nullptr != pLayer)	
		pLayer->Add_GameObject(pGameObject);
	else
	{
		if (nullptr == m_pMapLayer)
			return E_FAIL;

		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pMapLayer[iArrayIdx].insert(MAPLAYER::value_type(pLayerTag, pLayer));
	}

	return NOERROR;
}

HRESULT CObject_Manager::Ready_Object_Manager(void)
{
	return NOERROR;
}

_int CObject_Manager::Update_Object_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[i])		
		{
			iExitCode = Pair.second->Update_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}
		
	}
	return iExitCode;
}

_int CObject_Manager::LastUpdate_Object_Manager(const _float & fTimeDelta)
{
	_int		iExitCode = 0;

	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[i])
		{
			iExitCode = Pair.second->LastUpdate_Layer(fTimeDelta);
			if (iExitCode & 0x80000000)
				return iExitCode;
		}
	}
	return iExitCode;
}

void CObject_Manager::Render_Object_Manager(void)
{
	for (size_t i = 0; i < m_iContainerSize; ++i)
	{
		if (m_pMapLayer[i].empty())
			continue;

		for (auto& Pair : m_pMapLayer[i])		
			Pair.second->Render_Layer();
	}
}

CLayer * CObject_Manager::Find_Layer(const _uint & iArrayIdx, const _tchar * pLayerTag)
{
	if (nullptr == m_pMapLayer
		|| iArrayIdx >= m_iContainerSize)
		return nullptr;

	auto iter = find_if(m_pMapLayer[iArrayIdx].begin(), m_pMapLayer[iArrayIdx].end(), CFind_Tag(pLayerTag));

	if (iter == m_pMapLayer[iArrayIdx].end())
		return nullptr;

	return iter->second;
}

_ulong CObject_Manager::Free(void) //By.Jang
{
	_ulong dwRefCnt = 0;


	for (size_t i = 0; i < m_iContainerSize; i++)
	{
		for_each(m_pMapLayer[i].begin(), m_pMapLayer[i].end(), CRelease_Pair());
		m_pMapLayer[i].clear();
	}
	Safe_Delete_Array(m_pMapLayer);
	
	return dwRefCnt;
}
