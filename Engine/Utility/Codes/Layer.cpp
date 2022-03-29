#include "..\Headers\Layer.h"
#include "GameObject.h"

USING(Engine)

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _uint & iIndex, const _tchar * pComTag)
{
	_uint iSize = m_ObjectList.size();

	if (iIndex >= iSize)
		return nullptr;

	auto iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_ObjectList.end())
		return nullptr;


	return (*iter)->Get_Component(pComTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return NOERROR;
}

HRESULT CLayer::Ready_Layer(void)
{
	return NOERROR;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_ObjectList.begin();
	auto& iter_end = m_ObjectList.end();

	for ( ; iter != iter_end ; )
	{
		iExitCode = (*iter)->Update_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			Safe_Release((*iter));
			iter = m_ObjectList.erase(iter);
		}
		else if (iExitCode & 0x80000000) /* exit code < 0*/
		{
			return iExitCode;
		}
		else
			++iter;
	}

	return iExitCode;
}

_int CLayer::LastUpdate_Layer(const _float & fTimeDelta)
{
	_int iExitCode = 0;

	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end(); )
	{
		iExitCode = (*iter)->LastUpdate_GameObject(fTimeDelta);

		if (1 == iExitCode)	/*Object Is Dead*/
		{
			Safe_Release((*iter));
			iter = m_ObjectList.erase(iter);
		}
		else if (iExitCode & 0x80000000) /* exit code < 0*/
		{
			return iExitCode;
		}
		else
			++iter;
	}

	return iExitCode;
}

void CLayer::Render_Layer(void)
{
	for (auto& pGameObject : m_ObjectList)	
		pGameObject->Render_GameObject();	
}

void CLayer::Dead_AllObject()
{
	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end();  ++iter)
	{
		(*iter)->Set_IsLastDead();
	}
}

CLayer * CLayer::Create(void)
{
	CLayer *	pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MessageBox(0, L"CLayer Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}


_ulong CLayer::Free(void)	//By.Jang
{
	
	_ulong dwRefCnt = 0;

	for_each(m_ObjectList.begin(), m_ObjectList.end(), CRelease_Single());
	m_ObjectList.clear();

	return dwRefCnt;
}


