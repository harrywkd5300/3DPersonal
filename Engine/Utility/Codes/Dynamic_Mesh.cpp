#include "..\Headers\Dynamic_Mesh.h"
#include "Hierachy_Loader.h"
#include "Animation_Ctrl.h"

USING(Engine)

CDynamic_Mesh::CDynamic_Mesh(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vRota)
	: CMesh(pGraphicDev)
	, m_pLoader(nullptr)
	, m_pFrame(nullptr)
	, m_pAnimation_Ctrl(nullptr)
	, m_dwNumMeshContainer(0)
	, m_vRota(vRota)
{
}

CDynamic_Mesh::CDynamic_Mesh(const CDynamic_Mesh & rhs)
	: CMesh(rhs)
	, m_pFrame(rhs.m_pFrame)
	, m_dwNumMeshContainer(rhs.m_dwNumMeshContainer)
	, m_vecMeshContainer(rhs.m_vecMeshContainer)
	, m_pLoader(rhs.m_pLoader)
	, m_vRota(rhs.m_vRota)
	, m_pAnimation_Ctrl(CAnimation_Ctrl::Create(*rhs.m_pAnimation_Ctrl))   // 깊은 복사로 복사해준다.
{
}

const _vec3 * CDynamic_Mesh::Get_Min(const _uint & iMeshContainerIdx)
{
	if (iMeshContainerIdx >= m_vecMeshContainer.size())
		return nullptr;

	return &m_vecMeshContainer[iMeshContainerIdx]->vMin;
}

const _vec3 * CDynamic_Mesh::Get_Max(const _uint & iMeshContainerIdx)
{
	if (iMeshContainerIdx >= m_vecMeshContainer.size())
		return nullptr;

	return &m_vecMeshContainer[iMeshContainerIdx]->vMax;
}

HRESULT CDynamic_Mesh::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	_tchar		szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierachy_Loader::Create(m_pGraphicDev, pFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER			pAniCtrl = nullptr;   // 애니메이션컨트롤러 변수 선언

	// m_pFrame 에 정보를 채워주는 작업을 해준다. 
	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev, m_pLoader, nullptr,
		(D3DXFRAME**)&m_pFrame, &pAniCtrl)))      // 애니메이션 넣어준다.
		return E_FAIL;

	m_pAnimation_Ctrl = CAnimation_Ctrl::Create(m_pGraphicDev, pAniCtrl);   // 컨트롤러를 넣어준다.
	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	Safe_Release(pAniCtrl);

	Compute_NumMeshContainer(m_pFrame);  // Mesh Container의 사이즈를 미리 구해서 정해둔다.

	_matrix		matStartMatrix = *matStartMatrix.Identity();

	_matrix		matRotaX = *matRotaX.Identity();
	_matrix		matRotaY = *matRotaY.Identity();
	_matrix		matRotaZ = *matRotaZ.Identity();

	if(m_vRota.x > 0 )
		D3DXMatrixRotationX(&matRotaX, D3DXToRadian(m_vRota.x));   // 로드 하는 모든 메시에 X축으로 180도 회전시켜서 그린다.
	if (m_vRota.y > 0)
		D3DXMatrixRotationY(&matRotaY, D3DXToRadian(m_vRota.y));   // 로드 하는 모든 메시에 Y축으로 180도 회전시켜서 그린다.
	if (m_vRota.z > 0)
		D3DXMatrixRotationZ(&matRotaZ, D3DXToRadian(m_vRota.z));   // 로드 하는 모든 메시에 Z축으로 180도 회전시켜서 그린다.

	matStartMatrix = matRotaX * matRotaY * matRotaZ;

	Update_CombinedTransformaitionMatrix(m_pFrame, &matStartMatrix);

	m_vecMeshContainer.reserve(m_dwNumMeshContainer);

	if (FAILED(SetUp_BoneMatrixPointer(m_pFrame)))
		return E_FAIL;

	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		D3DXVec3TransformNormal(&m_vecMeshContainer[i]->vMin, &m_vecMeshContainer[i]->vMin, &matStartMatrix);
		D3DXVec3TransformNormal(&m_vecMeshContainer[i]->vMax, &m_vecMeshContainer[i]->vMax, &matStartMatrix);
	}


	return NOERROR;
}

void CDynamic_Mesh::Render_Mesh(void)
{
	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		for (size_t i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			// 뼈의 갯수만큼 순회하면서 RenderingMatrix 정보를 넣어준다.
			pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffsetMatrices[i] * *pMeshContainer->ppCombinedTransformationMatrices[i];
		}

		void*		pSrcVertex = nullptr, *pDestVertex = nullptr;

		pMeshContainer->pOriginal_Mesh->LockVertexBuffer(0, &pSrcVertex);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVertex);

		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pSrcVertex, pDestVertex);
		// 스키닝 : 뼈를 접거나 움직일때 피부부분이 살짝 짤리는데 이 부분을 채우는작업
		// 소프트웨어 스키닝 ==> 느리다, 나중에 하드웨어 스키닝으로 바뀜 ==> 쉐이더에서 작업

		pMeshContainer->pOriginal_Mesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

		// 변환된 정점들을 그린다.
		for (size_t i = 0; i < pMeshContainer->NumMaterials; i++)
		{
			//m_pGraphicDev->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
			m_pGraphicDev->SetTexture(0, pMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED]);
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}
	}
}

void CDynamic_Mesh::Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name)
{
	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		for (size_t i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffsetMatrices[i] * *pMeshContainer->ppCombinedTransformationMatrices[i];
		}

		void*		pSrcVertex = nullptr, *pDestVertex = nullptr;
		pMeshContainer->pOriginal_Mesh->LockVertexBuffer(0, &pSrcVertex);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVertex);

		// 메시를 이루는 정점을 뼈대들의 정보만큼 변환한다 + 스키닝 ㅐ
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pSrcVertex, pDestVertex);

		pMeshContainer->pOriginal_Mesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();



		// 변환된 정점들을 그린다.
		for (size_t i = 0; i < pMeshContainer->NumMaterials; i++)
		{
			//m_pGraphicDev->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
			//m_pGraphicDev->SetTexture(0, pMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSE]);
			pEffect->SetTexture(pConstant_Texture_Name, pMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED]);

			pEffect->CommitChanges();

			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}
	}
}

HRESULT CDynamic_Mesh::Set_AnimationSet(const _uint & iAni_Idx)
{
	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	m_pAnimation_Ctrl->Set_AnimationSet(iAni_Idx);

	return NOERROR;
}

void CDynamic_Mesh::Set_AnimationFirst()
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	m_pAnimation_Ctrl->Set_Animation();

	return;
}

void CDynamic_Mesh::Play_Animation(const _float & fDeltaTime, const _float& fAniTime)
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	m_pAnimation_Ctrl->Play_Animation(fDeltaTime, fAniTime);

	_matrix		matStartMatrix = *matStartMatrix.Identity();

	_matrix		matRotaX = *matRotaX.Identity();
	_matrix		matRotaY = *matRotaY.Identity();
	_matrix		matRotaZ = *matRotaZ.Identity();

	if (m_vRota.x > 0)
		D3DXMatrixRotationX(&matRotaX, D3DXToRadian(m_vRota.x));   // 로드 하는 모든 메시에 X축으로 180도 회전시켜서 그린다.
	if (m_vRota.y > 0)
		D3DXMatrixRotationY(&matRotaY, D3DXToRadian(m_vRota.y));   // 로드 하는 모든 메시에 Y축으로 180도 회전시켜서 그린다.
	if (m_vRota.z > 0)
		D3DXMatrixRotationZ(&matRotaZ, D3DXToRadian(m_vRota.z));   // 로드 하는 모든 메시에 Z축으로 180도 회전시켜서 그린다.

	matStartMatrix = matRotaX * matRotaY * matRotaZ;


	Update_CombinedTransformaitionMatrix(m_pFrame, &matStartMatrix);
}

D3DXMATRIX* CDynamic_Mesh::Get_BoneMatrix(const char* pCmpChar)
{
	return &((D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrame, pCmpChar))->matCombineTransformationMatrix;
}

_bool CDynamic_Mesh::End_Animation(const _double& Delay)
{
	return m_pAnimation_Ctrl->End_Animation(Delay);
}

HRESULT CDynamic_Mesh::Compute_NumMeshContainer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
		++m_dwNumMeshContainer;

	if (nullptr != pFrame->pFrameFirstChild)
	{
		Compute_NumMeshContainer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		Compute_NumMeshContainer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);
	}

	return NOERROR;
}

HRESULT CDynamic_Mesh::Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED * pFrame, const _matrix * pParentCombinedMatrix)
{
	pFrame->matCombineTransformationMatrix = pFrame->TransformationMatrix * *pParentCombinedMatrix;

	if (nullptr != pFrame->pFrameFirstChild)
	{
		// 뼈들의 자식들의 행렬을 곱해주는곳 // pFrame이 가지고 있는 combinematrix를 곱해준다.
		Update_CombinedTransformaitionMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, (_matrix*)&pFrame->matCombineTransformationMatrix);
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		// 뼈들의 형제들의 행렬을 곱해주는곳 // 같은 부모의 행렬을 곱해준다
		Update_CombinedTransformaitionMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentCombinedMatrix);
	}

	return NOERROR;
}

HRESULT CDynamic_Mesh::SetUp_BoneMatrixPointer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;  // 프레임안에 있는 컨테이너로 채워준다.

		for (size_t i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			const char*		pTmp = pMeshContainer->pSkinInfo->GetBoneName(i);    // 뼈의 이름을 받고

			D3DXFRAME_DERIVED*	pFrame_Find = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrame, pTmp);   
			// 최상위 뼈와 찾고자 하는이름을 넣으면 그 뼈의 주소를 반환한다.
			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFrame_Find->matCombineTransformationMatrix;

		}
		m_vecMeshContainer.push_back(pMeshContainer);
	}

	// pFrame 구조체 안에 컨테이너의 주소정보를 가지고 있으므로 재귀로 돌면서 Child와 Sibling을 채워준다.
	if (nullptr != pFrame->pFrameFirstChild)
	{
		if (FAILED(SetUp_BoneMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild)))
			return E_FAIL;
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		if (FAILED(SetUp_BoneMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling)))
			return E_FAIL;
	}

	return NOERROR;
}

CComponent * CDynamic_Mesh::Clone(void)
{
	return new CDynamic_Mesh(*this);
}

CDynamic_Mesh * CDynamic_Mesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName, _vec3 vRota)
{
	CDynamic_Mesh *	pInstance = new CDynamic_Mesh(pGraphicDev, vRota);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName)))
	{
		MSG_BOX("CMesh_Dynamic Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CDynamic_Mesh::Free(void)
{
	if (false == m_isClone)
	{
		m_pLoader->DestroyFrame(m_pFrame);

		Safe_Delete(m_pLoader);
	}

	m_vecMeshContainer.clear();

	Safe_Release(m_pAnimation_Ctrl);

	return CMesh::Free();
}
