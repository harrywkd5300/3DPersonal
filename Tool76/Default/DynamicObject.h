#pragma once

#include "GameObject.h"
#include "Tool_Define.h"


namespace Engine
{
	class CRenderer;
	class CTransform;
	class CDynamic_Mesh;
}

class CDynamicObject : public CGameObject
{
private:
	explicit CDynamicObject(LPDIRECT3DDEVICE9	pGraphicDev);
	virtual	~CDynamicObject() = default;

public:
	virtual HRESULT Ready_GameObject(TCHAR ComPath[128], const _vec3& vMakeScale) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	HRESULT				Ready_Component(void);
	HRESULT				Ready_MeshComponent(TCHAR ComPath[128]);

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CDynamic_Mesh*		m_pBufferCom = nullptr;

public:
	static CDynamicObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, TCHAR ComPath[128], const _vec3& vMakeScale);

private:
	virtual _ulong Free() final;

};

