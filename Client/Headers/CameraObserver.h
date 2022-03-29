#pragma once

#include "Defines.h"
#include "Observer.h"

namespace Engine
{
	class CTransform;
}


class CCameraObserver : public CObserver
{

	enum CAMERA_STATE { MESSAGE_PLAYERCAMERA, MESSAGE_END  };
private:
	explicit CCameraObserver();
	virtual ~CCameraObserver();

private:
	virtual HRESULT Ready_Observer(void); // ������ �غ��Ұ��� �ִٸ� 
	virtual void Update(int iMessage, void* pData);

public:
	CTransform* Get_Current_PlayerTransform() { return m_pCurrent_Transform; }

public:
	static CCameraObserver* Create();

private:
	CTransform* m_pCurrent_Transform = nullptr;

public:
	virtual _ulong Free(); // ������ ���� ��ü���� ���ٰ�.
};

