#pragma once

#include "Engine_Defines.h"
#include "Navigation.h"
#include "VIBuffer.h"

// 월드변환행렬의 제작 변환을 지속적으로 수행할 수 있게 해준다.

BEGIN(Engine)

class DLL_EXPORT CTransform : public CComponent
{
public:
	enum INFOMATION { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POSITION, INFO_END };
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

	/*FUNCTION*/
public:
	void	Make_WorldMatrix();
	void	SetUp_OnGraphicDev();
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName) {
		pEffect->SetMatrix(pConstantName, &m_matWorld);
	}

	void	Get_WorldMatrix(_matrix* matWorld, _matrix* matRot = nullptr) const;
	void	Get_WorldMatrix_NoRot(_matrix* matWorld, _matrix* matRot = nullptr) const;

	const _matrix&  Get_PostionMatrix() { return m_matTrans; }
	const _matrix&	Get_ScaleMatrix() { return m_matScale; };
	const _matrix&	Get_RotX() { return m_matRotationX; };
	const _matrix&	Get_RotY() { return m_matRotationY; };
	const _matrix&	Get_RotZ() { return m_matRotationZ; };

	const _float&	Get_DegreeX() { return m_fDegreeX; }
	const _float&	Get_DegreeY() { return m_fDegreeY; }
	const _float&	Get_DegreeZ() { return m_fDegreeZ; }

	const _float&	Get_PositionX() { return m_matTrans._41; }
	const _float&	Get_PositionY() { return m_matTrans._42; }
	const _float&	Get_PositionZ() { return m_matTrans._43; }

	_vec3	Get_Position() { return _vec3(m_matTrans._41, m_matTrans._42, m_matTrans._43); }
	_vec3	Get_Scale() { return _vec3(m_matScale._11, m_matScale._22, m_matScale._33); }
	_vec3	Get_Rotation() { return _vec3(m_fDegreeX, m_fDegreeY, m_fDegreeZ); }

	_vec3   Get_Look();
	_vec3	Get_Right();
	_vec3	Get_Up();
	void	Get_LookDgreeY(_float& fDgree,_vec3* vLook, _vec3* vMyLook);


	void	Set_DegreeX(const _float& fx) { m_fDegreeX = fx; Rotation_X(0); }
	void	Set_DegreeY(const _float& fy) { m_fDegreeY = fy; Rotation_Y(0); }
	void	Set_DegreeZ(const _float& fz) { m_fDegreeZ = fz; Rotation_Z(0); }
	void	Set_Degree(const _vec3& vRot);

	void	Set_PositionX(const _float& fX) { m_matTrans._41 = fX; }
	void	Set_PositionY(const _float& fY) { m_matTrans._42 = fY; }
	void	Set_PositionZ(const _float& fZ) { m_matTrans._43 = fZ; }
	void	Set_Position(const _vec3& vPos) { (_vec3&)m_matTrans.m[INFO_POSITION] = vPos; }

	void	Set_MatrixRow(const _vec3& v, INFOMATION eInfo) const { (_vec3&)m_matWorld.m[eInfo] = v; }
	void	Set_MatrixScale(_vec3& v) 
	{ 
		_vec3		vRight, vUp, vLook;

		// x_Scale
		memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));
		D3DXVec3Normalize(&vRight, &vRight);
		vRight *= v.x;
		memcpy(&m_matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));

		memcpy(&vUp, &m_matWorld.m[INFO_UP][0], sizeof(_vec3));
		D3DXVec3Normalize(&vUp, &vUp);
		vUp *= v.y;
		memcpy(&m_matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));

		memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));
		D3DXVec3Normalize(&vLook, &vLook);
		vLook *= v.z;
		memcpy(&m_matWorld.m[INFO_LOOK][0], &vLook, sizeof(_vec3));

		//m_matWorld._11 *= v.x;
		//m_matWorld._22 *= v.y;
		//m_matWorld._33 *= v.z;
	}

	void Scaling(const _vec3& vScale);
	void Scaling(const _float& x, const _float& y, const _float& z);

	void Rotation_X(const _float& fDegree);
	void Rotation_Y(const _float& fDegree);
	void Rotation_Z(const _float& fDegree);

	void BillBoard_RotationX(const _float& fAngle, const _float& fTimeDelta);
	void BillBoard_RotationY(const _float& fAngle, const _float& fTimeDelta);
	void BillBoard_RotationZ(const _float& fAngle, const _float& fTimeDelta);

	void Translation(const _vec3& vTranslation);
	void Translation(const _float& x, const _float& y, const _float& z);

	void Parent(const _matrix& matParent);

	void Move_ToDirection(const _vec3& vDirection, const _float& fSpeed, const _float& fTimeDelta);
	void Go_Straight(const _float& fSpeed, const _float& fTimeDelta);
	void Go_Right(const _float& fSpeed, const _float& fTimeDelta);
	void Sliding(const _float& fSpeed, const _float& fTimeDelta, const _vec3& vNormal);

	void Get_SlidingVec(_vec3* vSliding, const _float& fSpeed, const _float& fTimeDelta, const _vec3& vNormal);
	void Get_NextMove(_vec3* vSliding, const _vec3* vLook);

public:
	_float Compute_Distance(const _matrix* pWorldMatrix);
	_float Compute_Distance(const _vec3* vPosition);
	void Move_OnBuffer(Engine::CVIBuffer* pTargetBuffer);
	void Move_OnNavigation(Engine::CNavigation* pNavigation);
	void Get_CurPositionY(Engine::CNavigation * pNavigation, _float* fY);

public:
	HRESULT Ready_Transform(void);

public:
	virtual void Update_Component(const _float& fTimeDelta);

protected:
	_matrix		m_matWorld;
	_matrix		m_matScale;
	_matrix		m_matRotationX;
	_matrix		m_matRotationY;
	_matrix		m_matRotationZ;
	_matrix		m_matTrans;
	_matrix		m_matParent;

	_vec3		m_vLook;

	_float		m_fDegreeX = 0.f;
	_float		m_fDegreeY = 0.f;
	_float		m_fDegreeZ = 0.f;

private:
	virtual CComponent* Clone(void);
public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void);
};

END