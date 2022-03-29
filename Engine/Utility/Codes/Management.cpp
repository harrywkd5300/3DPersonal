#include "..\Headers\Management.h"
#include "Scene.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pCurrent_Scene(nullptr)
	, m_pNew_Scene(nullptr)
{
	
}

CManagement::~CManagement()
{

}

HRESULT CManagement::SetUp_CurrentScene(CScene * pNewScene)
{
	if (nullptr == pNewScene)
		return E_FAIL;

	_ulong dwRefCnt = 0;

	if (nullptr != m_pCurrent_Scene)
	{
		m_pNew_Scene = pNewScene;
	/*	*/
	}
	else
		m_pCurrent_Scene = pNewScene;

	return NOERROR;
}


HRESULT CManagement::SetUp_Renderer(CRenderer * pRenderer)
{
	if (nullptr != m_pRenderer)
		return E_FAIL;

	m_pRenderer = pRenderer;
	m_pRenderer->AddRef();

	return NOERROR;
}

HRESULT CManagement::Ready_Management(const _uint& iMaxSize)
{
	// 인게임 구성에 필요한 매니져들의 공간을 확보하는 작업을 수행한다.	
	if (FAILED(CComponent_Manager::GetInstance()->Reserve_ComponentContainer(iMaxSize)))	
		return E_FAIL;

	if (FAILED(CObject_Manager::GetInstance()->Reserve_ObjectContainer(iMaxSize)))
		return E_FAIL;
	
	return NOERROR;
}

_int CManagement::Update_Management(const _float & fTimeDelta)
{
	if (nullptr == m_pCurrent_Scene)
		return -1;

	// 다음씬으로 교체될 준비가 되어있다.
	_ulong			dwRefCnt = 0;
	if(nullptr != m_pNew_Scene)
	{
		if (dwRefCnt = Engine::Safe_Release(m_pCurrent_Scene))
			return -1;
		m_pCurrent_Scene = m_pNew_Scene;
		m_pNew_Scene = nullptr;
	}
	_int		iExitCode = 0;
	iExitCode = m_pCurrent_Scene->Update_Scene(fTimeDelta);
	if (iExitCode & 0x80000000)
		return iExitCode;

	iExitCode = m_pCurrent_Scene->LastUpdate_Scene(fTimeDelta);
	if (iExitCode & 0x80000000)
		return iExitCode;

	return 0;
}

void CManagement::Render_Management(void)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Render_RenderGroup();
	// Renderer는 Scene에서 하는 방법도 있음 ==> Ready때 생성한 씬에 Renderer를 보내고 
	// 그 Renderer를 Scene을 Render해준다.!!
	if(nullptr != m_pCurrent_Scene)
		m_pCurrent_Scene->Render_Scene();
}

_ulong CManagement::Free(void)
{
	_ulong dwRefCnt = 0;

	Engine::Safe_Release(m_pRenderer);	

	if (dwRefCnt = Engine::Safe_Release(m_pCurrent_Scene))
		MSG_BOX("Current_Scene Release Failed");

	return dwRefCnt;
}
