#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Triangle_Color.h"
#include "Rect_Color.h"
#include "Rect_Texture.h"
#include "Terrain_Buffer.h"
#include "Renderer.h"
#include "Texture.h"
#include "Transform.h"
#include "Material.h"
#include "Cube_Texture.h"
#include "Picking.h"
#include "RectUI_Texture.h"
#include "Static_Mesh.h"
#include "Dynamic_Mesh.h"
#include "Collider.h"
#include "Navigation.h"
#include "Shader.h"
#include "Frustum.h"
#include "SnowPaticle.h"
#include "BossPaticle.h"


BEGIN(Engine)

// 용도 : 객체들을 렌더링 또는 사용함에 있어 필요한 부품객체들을 모아서 관리한다.


class DLL_EXPORT CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT		Ready_Component(const _uint& iArrayIdx, const _tchar* pComponentTag, CComponent* pComponent);
	//HRESULT		Add_Component(const _uint& iSceneIdx, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Component(const _uint& iArrayIdx, const _tchar* pComponentTag);
public:
	HRESULT		Reserve_ComponentContainer(const _uint& iMaxSize);
	HRESULT		Release_Component(const _uint& iSceneIdx);
	HRESULT		Release_TargetComponent(const _uint& iSceneIdx, const _tchar * pComponentTag);
private:
	map<const _tchar*, CComponent*>*			m_pMapComponents = nullptr;
	typedef map<const _tchar*, CComponent*>		MAPCOMPONENTS;
private:
	_uint				m_iContainerSize = 0;
public:
	CComponent* Find_Component(const _uint& iArrayIdx, const _tchar* pComponentTag);
private:
	virtual _ulong Free(void) final;


};

END