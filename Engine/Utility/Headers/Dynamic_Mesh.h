#pragma once

#include "Engine_Defines.h"
#include "Mesh.h"

BEGIN(Engine)

class CAnimation_Ctrl;
class CHierachy_Loader;
class DLL_EXPORT CDynamic_Mesh final : public CMesh
{
private:
	explicit CDynamic_Mesh(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vRota);
	explicit CDynamic_Mesh(const CDynamic_Mesh& rhs);
	virtual ~CDynamic_Mesh() = default;

public:
	const _vec3* Get_Min(const _uint& iMeshContainerIdx = 0);
	const _vec3* Get_Max(const _uint& iMeshContainerIdx = 0);

public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);
	virtual void Render_Mesh(void);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name);
public:
	HRESULT		Set_AnimationSet(const _uint& iAni_Idx);   
	void		Set_AnimationFirst();
	void		Play_Animation(const _float& fDeltaTime, const _float& fAniTime = 1.f);
	D3DXMATRIX*	Get_BoneMatrix(const char* pCmpChar);
	D3DXFRAME_DERIVED*	Get_Frame() { return m_pFrame; }

	_bool		End_Animation(const _double& Delay);

private:
	CHierachy_Loader*					m_pLoader = nullptr;
	D3DXFRAME_DERIVED*					m_pFrame = nullptr;

	_ulong								m_dwNumMeshContainer = 0;

	CAnimation_Ctrl*					m_pAnimation_Ctrl = nullptr;
	vector<D3DXMESHCONTAINER_DERIVED*>	m_vecMeshContainer;
	typedef vector<D3DXMESHCONTAINER_DERIVED*>	VECMESHCONTAINER;

	_vec3								m_vRota;

private:
	HRESULT	Compute_NumMeshContainer(D3DXFRAME_DERIVED*	pFrame);
	HRESULT Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix);
	HRESULT SetUp_BoneMatrixPointer(D3DXFRAME_DERIVED* pFrame);

public:
	virtual CComponent* Clone(void);
	static CDynamic_Mesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName, _vec3 vRota);
private:
	virtual _ulong Free(void);
};

END

