#include "..\Headers\Hierachy_Loader.h"

USING(Engine)

CHierachy_Loader::CHierachy_Loader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath)
	: m_pGraphicDev(pGraphicDev)
	, m_pPath(pPath)
{
}

STDMETHODIMP CHierachy_Loader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	if (nullptr != Name)
		Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->matCombineTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return NOERROR;
}

STDMETHODIMP CHierachy_Loader::CreateMeshContainer(LPCSTR Name, 
												   CONST D3DXMESHDATA * pMeshData, 
												   CONST D3DXMATERIAL * pMaterials, 
												   CONST D3DXEFFECTINSTANCE * pEffectInstances, 
												   DWORD NumMaterials, 
												   CONST DWORD * pAdjacency, 
											       LPD3DXSKININFO pSkinInfo, 
												   LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*			pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	// 이름 정보를 채워준다.
	if (nullptr != Name) 
		Allocate_Name(&pNewMeshContainer->Name, Name);

	// Mesh 정보를 채워준다.
	if (nullptr == pMeshData)
		return E_FAIL;

	pNewMeshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
	//D3DXMESHDATA pMeshData에 있는 구조체에 들어가면 선언되어 있는 타입, 지금만 기본 메시타입을 사용한다.
	pNewMeshContainer->MeshData.pMesh = pMeshData->pMesh;
	pNewMeshContainer->MeshData.pMesh->AddRef();

	// 편리함을 위하여 잠시 담아두는 역활
	LPD3DXMESH		pMesh = pNewMeshContainer->MeshData.pMesh;
	pMesh->AddRef();

	_ulong dwNumFaces = pMesh->GetNumFaces();
	// GetNumFaces : Mesh에 포함되 있는 면(폴리곤)의 갯수를 반환해준다.

	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	// 폴리곤에 인접해있는 면의 갯수는 3개이니 위에서 구한 dwNumFaces인 면의 갯수에서 3개를 곱해준다.
	// pAdjacency : 폴리곤에 인접해있는 면의 주소
	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);
	// 위에서 받은 pAdjacency를 넣어준다.

	_ulong dwMeshFVF = pMesh->GetFVF();

	if (!(dwMeshFVF & D3DFVF_NORMAL))
		D3DXComputeNormals(pMesh, pNewMeshContainer->pAdjacency);
	// 노말벡터가 정점의 정보에 없으면 구해서 넣어준다. D3DXComputeNormals : 노말벡터를 구해준다.

	D3DVERTEXELEMENT9		Vertex_Element[MAX_FVF_DECL_SIZE];
	// MAX_FVF_DECL_SIZE : 정점 하나가 가질수 있는 최대 FVF의 개수.

	ZeroMemory(Vertex_Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);  // 추가 

	pMesh->GetDeclaration(Vertex_Element);

	_ushort		wOffset = 0;

	//  *Stream // 디버그 해보았을때 255전에 Stream 들만 유효한 FVF이다.
	//		    // 255는 끝을 알리는것
	//	*Offset // 앞에서부터 얼마의 메모리 뒤에 있는지를 알려주는 정보변수
	//	*Type   // DataType // ex ) Float, Color
	//	*Usage  // 어떤 FVF 인가를 지정해주는 변수
	//	*UsageIndex // 혹여나 같은 FVF가 여러개 들어가있을때 몇번째인가를 알려주는 변수

	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Vertex_Element[i].Usage)  // FVF가 POSITION인 경우
		{
			wOffset = Vertex_Element[i].Offset;    // 얼마만큼의 메모리가 떨어져 있는지
			break;
		}
	}

	void*		pVertex = nullptr;

	pMesh->LockVertexBuffer(0, &pVertex);

	_ulong dwNumVertices = pMesh->GetNumVertices();  // mesh에 포함되어 있는 정점의 갯수

	_uint  iStride = D3DXGetFVFVertexSize(dwMeshFVF);  // 특정정보를 가진 정점 포맷에 저장 되고 있는 정점의 사이즈

	D3DXComputeBoundingBox((_vec3*)((_byte*)pVertex + wOffset), dwNumVertices, iStride, &pNewMeshContainer->vMin, &pNewMeshContainer->vMax);
	// mesh의 FVF를 통하여 POSITION으로 부터 바운딩 박스를 생성하기 위한 min값과 max값을 저장한다.

	pMesh->UnlockVertexBuffer();

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL_DERIVED[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMaterials, sizeof(D3DXMATERIAL_DERIVED) * pNewMeshContainer->NumMaterials); // 변경

	pNewMeshContainer->pMeshTexture = new MESHTEXTURE[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMeshTexture, sizeof(MESHTEXTURE) * pNewMeshContainer->NumMaterials);

	if (0 != NumMaterials)
	{
		// 재질과 텍스쳐 저장 
		for (size_t i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			pNewMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;   // 재질을 저장
			pNewMeshContainer->pMaterials[i].pTextureFilename = pMaterials[i].pTextureFilename;  // 파일이름을 저장한다.

			_tchar		szFullPath[128] = L"";
			_tchar		szFileName[128] = L"";

			lstrcpy(szFullPath, m_pPath);					// 변수로 받은 경로를 저장한다

			// static mesh와 같은 형태 알파값이 들어간 mesh의 경우에 따로 처리한다.
			((D3DXMATERIAL_DERIVED*)&pNewMeshContainer->pMaterials[i])->isAlpha = isAlpha(pNewMeshContainer->pMaterials[i].pTextureFilename, "A");
	
			MultiByteToWideChar(CP_ACP, 0, pNewMeshContainer->pMaterials[i].pTextureFilename, strlen(pNewMeshContainer->pMaterials[i].pTextureFilename)
				, szFileName, 128);

			lstrcat(szFullPath, szFileName);

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_DIFFUSED])))
				return E_FAIL;
		
			// For.Normal Texture
			lstrcpy(szFullPath, m_pPath);
			Convert_KeyWordTextureName(pNewMeshContainer->pMaterials[i].pTextureFilename, "N", szFileName);
			lstrcat(szFullPath, szFileName);
			D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_NORMAL]);

			// For.Specualr Texture
			lstrcpy(szFullPath, m_pPath);
			Convert_KeyWordTextureName(pNewMeshContainer->pMaterials[i].pTextureFilename, "S", szFileName);
			lstrcat(szFullPath, szFileName);
			D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[MESHTEXTURE::TEX_SPECULAR]);
		}
	}

	if (nullptr == pSkinInfo)
		return E_FAIL;

	pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF(), m_pGraphicDev, &pNewMeshContainer->pOriginal_Mesh);
	// mesh FVF를 복제하는 함수, 인자 1: Mesh 생성 옵션 / 인자 2 : Mesh의 FVF / 인자 3 : 장치 / 인자 4 : 복제될 Mesh의 주소

	pNewMeshContainer->pSkinInfo = pSkinInfo;   // 받은 매개변수를 넣어준다.
	pNewMeshContainer->pSkinInfo->AddRef();     

	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones();  // Mesh에 영향을 미치고 있는 뼈의 갯수를 받는다

	pNewMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pNewMeshContainer->dwNumBones]; 
	// 여러가지의 뼈의 행렬정보의 주소를 보관하기 위하여 동적할당을 한다
	pNewMeshContainer->pOffsetMatrices = new _matrix[pNewMeshContainer->dwNumBones];   
	// 뼈들의 정점이 원점에서 얼마만큼 움직이는지에 대한 행렬의 정보를 보관하기 위하여 동적할당 
	pNewMeshContainer->pRenderingMatrices = new _matrix[pNewMeshContainer->dwNumBones];
	// Render 하기 위한 정보를 보관하기 위하여 동적할당

	for (size_t i = 0; i < pNewMeshContainer->dwNumBones; i++)
	{
		pNewMeshContainer->pOffsetMatrices[i] = *(pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
		// 뼈들의 offsetMatrices정보들을 보관한다.
	}

	Engine::Safe_Release(pMesh);

	*ppNewMeshContainer = pNewMeshContainer;

	return NOERROR;
}

STDMETHODIMP CHierachy_Loader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	Safe_Delete(pFrameToFree);

	return NOERROR;
}

STDMETHODIMP CHierachy_Loader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		for (size_t j = 0; j < MESHTEXTURE::TEX_END; ++j)
		{
			if (nullptr != pMeshContainer->pMeshTexture[i].pTexture[j])
				Safe_Release(pMeshContainer->pMeshTexture[i].pTexture[j]);
		}
	}

	Safe_Delete_Array(pMeshContainer->Name);

	Safe_Release(pMeshContainer->MeshData.pMesh);

	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);

	Safe_Release(pMeshContainer->pSkinInfo);

	Safe_Delete_Array(pMeshContainer->pMeshTexture);
	Safe_Release(pMeshContainer->pOriginal_Mesh);
	Safe_Delete_Array(pMeshContainer->pOffsetMatrices);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrices);
	Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);

	Safe_Delete(pMeshContainer);

	return NOERROR;
}

_bool CHierachy_Loader::isAlpha(const char * pFileName, const char * pKeyWord)
{
	_uint iLength = strlen(pFileName);

	for (size_t i = iLength - 1; i >= 0; --i)
	{
		if ('.' == pFileName[i])
		{
			if (*pKeyWord == pFileName[--i])
				return true;
			else
				break;
		}
	}
	return false;
}

void CHierachy_Loader::Convert_KeyWordTextureName(const char * pFileName, const char * pKeyWord, _tchar* szFullName)
{
	char		szFileName[128]  = "";
	char		szExtention[128] = "";

	_uint		iLength = strlen(pFileName);

	for (size_t i = iLength - 1; i >= 0; --i)
	{
		if ('.' == pFileName[i])
		{
			memcpy(szFileName, pFileName, sizeof(char) * (i - 1));

			strcat(szFileName, pKeyWord);

			memcpy(szExtention, pFileName + i, sizeof(char) * (iLength - i));

			strcat(szFileName, szExtention);

			break;
		}
	}

	MultiByteToWideChar(CP_ACP, 0, szFileName, strlen(szFileName)
		, szFullName, 128);
}

HRESULT CHierachy_Loader::Allocate_Name(char ** ppDestName, const char * const pName)
{
	size_t iLength = strlen(pName);

	*ppDestName = new char[iLength + 1];

	strcpy(*ppDestName, pName);

	return NOERROR;
}

CHierachy_Loader * CHierachy_Loader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath)
{
	CHierachy_Loader *	pInstance = new CHierachy_Loader(pGraphicDev, pFilePath);

	return pInstance;
}
