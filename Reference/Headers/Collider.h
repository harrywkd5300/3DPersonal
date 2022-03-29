#pragma once

#include "VIBuffer.h"
#include "Transform.h"

BEGIN(Engine)

class DLL_EXPORT CCollider final : public CVIBuffer
{
	enum COLLIDER_STATE { COLLIDER_COL, COLLIDER_NOCOL, COLLIDER_END };
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_CIRCLE };

private:
	typedef struct tagOBB
	{
		D3DXVECTOR3		vPoint[8];
		D3DXVECTOR3		vAlign_Axis[3];
		D3DXVECTOR3		vProj_Axis[3];
		D3DXVECTOR3		vCenter;
	}OBB;

private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:		// Getter
	_vec3	Get_Min() { return m_vMin; }
	_vec3	Get_Max() { return m_vMax; }

	_vec3	 Get_OriginMin() { return m_vOriginMin; }
	_vec3	 Get_OriginMax() { return m_vOriginMax; }

	void	Get_WorldMatrix(_matrix* matWorld, _matrix* StartMatrix = nullptr) { return m_pTransform->Get_WorldMatrix(matWorld, StartMatrix); }
	void	Get_WorldMatrix_NotRot(_matrix* matWorld, _matrix* StartMatrix = nullptr) { return m_pTransform->Get_WorldMatrix_NoRot(matWorld, StartMatrix); }

	_bool	Get_IsColl() { return m_isColl; }

	const CTransform* GetTransform() const { return m_pTransform; }

	_vec3*  Get_CollNormal() { return &m_pTargetNormal; }

	const _float&	Get_Radous() { return m_fRadios; }

public: // Setter
	void Set_Coll(_bool isColl) {
		m_isColl = isColl;
	}

	void Set_TargetNormal(_vec3* vNormal) {
		m_pTargetNormal = *vNormal;
	}

	void Set_NotColl(_bool	Not) {
		m_bNext = Not;
	}

	void Set_CircleMatrix(const _matrix& matCircle) {
		m_Circle = matCircle;
	}

	void Set_CircleScale(const _vec3& matCircle) {
		matScale._11 = matCircle.x;
		matScale._22 = matCircle.y;
		matScale._33 = matCircle.z;
	}

public:
	void	Set_ColliderSetting(TYPE eType, const _vec3* vMin, const _vec3* vMax, const CTransform*	pTransform, const _vec3* vRot = &_vec3(0.f, 0.f, 0.f));

public:
	HRESULT			Ready_Colider();
	void			Render_Buffer();
	void			Render_CircleBuffer(_vec3* vScale);

	_bool Collision_AABB(CCollider* pTargetCollider);
	_bool Collision_OBB(CCollider* pTargetCollider);
	_bool Collision_CIRCLE(CCollider* pTargetCollider);

	_matrix*	Get_StartMatrix() { return &matStartMatrix; }

	void		Set_Radius(CCollider * pTargetCollider);

	_vec3*		Get_Radius() { return &m_vRadius; }

private:
	TYPE					m_eType = TYPE_AABB;
	_vec3					m_vMin, m_vMax;
	_vec3					m_vOriginMin, m_vOriginMax;
	_vec3					m_vRot;
	LPDIRECT3DTEXTURE9		m_pTexture[COLLIDER_END];
	_bool					m_isColl = false;
	const CTransform*		m_pTransform = nullptr;

	_vec3					m_pTargetNormal;
	_bool					m_bNext = false;

	_matrix					matStartMatrix;

	_float					m_fRadios = 0.f;

	_matrix					m_Circle;
	_vec3					m_vCircleMin, m_vCircleMax;
	_vec3					m_vOriCircleMin, m_vOriCircleMax;
	_matrix					matScale;

	_vec3					m_vRadius = _vec3(0.f, 0.f, 0.f);

private:
	void		Set_Point(OBB*	pOBB, const _vec3* pMin, const _vec3* pMax);
	void		Set_Axis(OBB*	pOBB);

public:
	static CCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual  CComponent* Clone();
private:
	virtual _ulong Free();
};

END

