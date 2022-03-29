#pragma once

#include "gameObject.h"
#include "Defines.h"


namespace Engine
{
	class CRenderer;
	class CTransform;
	class CStatic_Mesh;
	class CCollider;
	class CShader;
	class CMaterial;
}

class CWave_Manager;
class CArrow final : public CGameObject
{
private:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrow() = default;


public:
	virtual HRESULT Ready_GameObject(_vec3* vStartPos, _vec3* vDir, _vec3* vRot, SCENETYPE eType) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CStatic_Mesh*		m_pBufferCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMaterial*			m_pMaterialCom = nullptr;
	CWave_Manager*		m_pWave_Manager = nullptr;

	_vec3				m_vDir;
	_float				m_fDist;

	SCENETYPE			m_eType = SCENE_END;
private:
	HRESULT		Ready_Component();
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void	Collision_ToTarget();
public:
	static CArrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* vStartPos, _vec3* vDir, _vec3* vRot, SCENETYPE eType);

protected:
	virtual _ulong Free() final;

};

