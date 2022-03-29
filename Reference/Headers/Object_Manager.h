#pragma once

// 객체들을 모아서관리하낟
// 클라이언트에서 지정한 갯수만큽ㅁ의 배열 컨테이너를 가진다.

#include "Engine_Defines.h"
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CLayer;
class CComponent;
class DLL_EXPORT CObject_Manager : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager();
public:
	Engine::CComponent* Get_Component(_uint iSceneIdx, const _tchar* pLayerTag, const _uint& iIndex, const _tchar* pComTag);
public:
	HRESULT Reserve_ObjectContainer(const _uint&  iMaxSize);
	HRESULT Release_Object(const _uint& iSceneIdx);
	HRESULT	Release_TargetLayer(const _uint& iSceneIdx, const _tchar * pLayerTag);
public:
	HRESULT Ready_GameObject(const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pGameObject);
public:
	HRESULT Ready_Object_Manager(void);
	_int Update_Object_Manager(const _float& fTimeDelta);
	_int LastUpdate_Object_Manager(const _float& fTimeDelta);
	void Render_Object_Manager(void); // 나중에 얘 사라짐ㅋ
private:
	map<const _tchar*, CLayer*>*			m_pMapLayer = nullptr;
	typedef map<const _tchar*, CLayer*>		MAPLAYER;
private:
	_uint									m_iContainerSize = 0;
public:
	CLayer* Find_Layer(const _uint& iArrayIdx, const _tchar* pLayerTag);
private:
	virtual _ulong Free(void) final;
};

END