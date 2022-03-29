#include "..\Headers\Camera_Manager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	:m_pCurCamera(nullptr)
{
}


CCamera * CCamera_Manager::Get_Camera(const _tchar * pCameraTag)
{
	CCamera*	pCamera = Find_Camera(pCameraTag);
	if (nullptr == pCamera)
		return nullptr;

	return pCamera;
}

CCamera * CCamera_Manager::Get_CurCamera()
{
	return m_pCurCamera;
}

HRESULT CCamera_Manager::Add_Camera(const _tchar * pCameraTag, CGameObject * pGameObject)
{
	/*check tag*/
	CCamera* pCamera = Find_Camera(pCameraTag);

	if (nullptr != pCamera)
		goto exception;

	/*check camera*/
	pCamera = dynamic_cast<CCamera*>(pGameObject);

	if (nullptr == pCamera)
		goto exception;


	m_pMapCamera.insert(make_pair(pCameraTag, pCamera));

	return NOERROR;

exception:
	MSG_BOX("Add Camera failed err:09954");
	return E_FAIL;
}

HRESULT CCamera_Manager::Change_Camera(const _tchar * pCameraTag)
{
	CCamera* pFindCamera = Find_Camera(pCameraTag);

	if (nullptr == pFindCamera)
		goto exception;

	m_pCurCamera = pFindCamera;

	return NOERROR;

exception:
	MSG_BOX("Change camera failed err:88747");
	return E_FAIL;
}

_int CCamera_Manager::Update_Camera_Manager(const _float & fTimeDelta)
{
	if (nullptr == m_pCurCamera)
		return -1;

	m_pCurCamera->Update_GameObject(fTimeDelta);
	return 0;
}

_int CCamera_Manager::LastUpdate_Camera_Manager(const _float & fTimeDelta)
{
	if (nullptr == m_pCurCamera)
		return -1;

	m_pCurCamera->LastUpdate_GameObject(fTimeDelta);

	return 0;
}

CCamera * CCamera_Manager::Find_Camera(const _tchar * pCameraTag)
{
	auto iter = find_if(m_pMapCamera.begin(), m_pMapCamera.end(), [&](const MAPCAMERA::value_type Pair) {return !lstrcmp(Pair.first, pCameraTag); });

	if (iter == m_pMapCamera.end())
		return nullptr;

	return iter->second;
}

_ulong CCamera_Manager::Free(void)
{
	_ulong dwRefCount = 0;

	for (auto& Pair : m_pMapCamera)
	{
		if (dwRefCount = Safe_Release(Pair.second))
		{
			MSG_BOX("Camera release failed");
			return dwRefCount;
		}
	}

	return dwRefCount;
}
