#pragma once

#include "Engine_Defines.h"

#include "Base.h"

// ���� �ΰ��ӿ� ��������� ���� �ּҸ� ������ �ְ�, ����� �������� ����ϳ�.

BEGIN(Engine)

class CScene;
class CRenderer;
class DLL_EXPORT CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement();
	virtual ~CManagement();
public:
	HRESULT SetUp_CurrentScene(CScene* pNewScene);
	HRESULT SetUp_Renderer(CRenderer* pRenderer);
public:
	HRESULT Ready_Management(const _uint& iMaxSize);
	_int Update_Management(const _float& fTimeDelta);
	void Render_Management(void);
private:
	Engine::CScene*				m_pCurrent_Scene = nullptr;
	Engine::CScene*				m_pNew_Scene = nullptr;
	Engine::CRenderer*			m_pRenderer = nullptr;

public:
	CScene*	Get_CurrentScene() { return m_pCurrent_Scene; }


private:
	virtual _ulong Free(void) final;
};

END