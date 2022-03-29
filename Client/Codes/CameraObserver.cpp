#include "stdafx.h"
#include "..\Headers\CameraObserver.h"

#include "Subject_Manager.h"

#include "Component_Manager.h"

#include "Player.h"
#include "Camera.h"

CCameraObserver::CCameraObserver()
{
}

CCameraObserver::~CCameraObserver()
{
}


HRESULT CCameraObserver::Ready_Observer(void)
{
	return NOERROR;
}

void CCameraObserver::Update(int iMessage, void * pData)
{
	list<void*>* pDataList = nullptr;
	list<void*>::iterator iter;

	pDataList = CSubject_Manager::GetInstance()->GetDataList(iMessage);

	if (pDataList == NULL)
		return;

	iter = find(pDataList->begin(), pDataList->end(), pData);

	if (iter == pDataList->end())
		return;

	switch (iMessage)
	{
	case MESSAGE_PLAYERCAMERA:
		m_pCurrent_Transform = ((CPlayer*)(*iter))->Get_Transform();
		break;
	}
}

CCameraObserver * CCameraObserver::Create()
{
	CCameraObserver *	pInstance = new CCameraObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX("CCameraObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCameraObserver::Free()
{
	return _ulong();
}
