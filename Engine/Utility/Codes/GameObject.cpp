#include "..\Headers\GameObject.h"

USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();

}



CComponent * CGameObject::Get_Component(const _tchar * pComTag)
{
	CComponent* pComponent = Find_Component(pComTag);

	if (nullptr == pComponent)
		return nullptr;
		
	return pComponent;
}

void CGameObject::Delete_Component(const _tchar * pComTag)
{
	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		auto& iter = find_if(m_mapComponent[i].begin(), m_mapComponent[i].end(), CFind_Tag(pComTag));
		if (iter != m_mapComponent[i].end())
		{
			Safe_Release(iter->second);
			m_mapComponent[i].erase(iter->first);
		}
	}
}

HRESULT CGameObject::Ready_GameObject(void)
{
	return NOERROR;
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	for (auto& Pair : m_mapComponent[Engine::CComponent::TYPE_DYNAMIC])
	{
		Pair.second->Update_Component(fTimeDelta);
	}
	return 0;
}

_int CGameObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return 0;
}

void CGameObject::Render_GameObject(void)
{
}

CComponent * CGameObject::Find_Component(const _tchar * pComTag)
{
	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		auto& iter = find_if(m_mapComponent[i].begin(), m_mapComponent[i].end(), CFind_Tag(pComTag));
		if (iter != m_mapComponent[i].end())
			return iter->second;
	}
	return nullptr;
}

_ulong CGameObject::Free(void)
{
	_ulong dwRefCnt = 0;

	for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CRelease_Pair());
		m_mapComponent[i].clear();
	}	

	Engine::Safe_Release(m_pGraphicDev);
	
	return dwRefCnt;
}
