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

	// �̸� ������ ä���ش�.
	if (nullptr != Name) 
		Allocate_Name(&pNewMeshContainer->Name, Name);

	// Mesh ������ ä���ش�.
	if (nullptr == pMeshData)
		return E_FAIL;

	pNewMeshContainer->MeshData.Type  = D3DXMESHTYPE_MESH;
	//D3DXMESHDATA pMeshData�� �ִ� ����ü�� ���� ����Ǿ� �ִ� Ÿ��, ���ݸ� �⺻ �޽�Ÿ���� ����Ѵ�.
	pNewMeshContainer->MeshData.pMesh = pMeshData->pMesh;
	pNewMeshContainer->MeshData.pMesh->AddRef();

	// ������ ���Ͽ� ��� ��Ƶδ� ��Ȱ
	LPD3DXMESH		pMesh = pNewMeshContainer->MeshData.pMesh;
	pMesh->AddRef();

	_ulong dwNumFaces = pMesh->GetNumFaces();
	// GetNumFaces : Mesh�� ���Ե� �ִ� ��(������)�� ������ ��ȯ���ش�.

	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	// �����￡ �������ִ� ���� ������ 3���̴� ������ ���� dwNumFaces�� ���� �������� 3���� �����ش�.
	// pAdjacency : �����￡ �������ִ� ���� �ּ�
	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);
	// ������ ���� pAdjacency�� �־��ش�.

	_ulong dwMeshFVF = pMesh->GetFVF();

	if (!(dwMeshFVF & D3DFVF_NORMAL))
		D3DXComputeNormals(pMesh, pNewMeshContainer->pAdjacency);
	// �븻���Ͱ� ������ ������ ������ ���ؼ� �־��ش�. D3DXComputeNormals : �븻���͸� �����ش�.

	D3DVERTEXELEMENT9		Vertex_Element[MAX_FVF_DECL_SIZE];
	// MAX_FVF_DECL_SIZE : ���� �ϳ��� ������ �ִ� �ִ� FVF�� ����.

	ZeroMemory(Vertex_Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);  // �߰� 

	pMesh->GetDeclaration(Vertex_Element);

	_ushort		wOffset = 0;

	//  *Stream // ����� �غ������� 255���� Stream �鸸 ��ȿ�� FVF�̴�.
	//		    // 255�� ���� �˸��°�
	//	*Offset // �տ������� ���� �޸� �ڿ� �ִ����� �˷��ִ� ��������
	//	*Type   // DataType // ex ) Float, Color
	//	*Usage  // � FVF �ΰ��� �������ִ� ����
	//	*UsageIndex // Ȥ���� ���� FVF�� ������ �������� ���°�ΰ��� �˷��ִ� ����

	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Vertex_Element[i].Usage)  // FVF�� POSITION�� ���
		{
			wOffset = Vertex_Element[i].Offset;    // �󸶸�ŭ�� �޸𸮰� ������ �ִ���
			break;
		}
	}

	void*		pVertex = nullptr;

	pMesh->LockVertexBuffer(0, &pVertex);

	_ulong dwNumVertices = pMesh->GetNumVertices();  // mesh�� ���ԵǾ� �ִ� ������ ����

	_uint  iStride = D3DXGetFVFVertexSize(dwMeshFVF);  // Ư�������� ���� ���� ���˿� ���� �ǰ� �ִ� ������ ������

	D3DXComputeBoundingBox((_vec3*)((_byte*)pVertex + wOffset), dwNumVertices, iStride, &pNewMeshContainer->vMin, &pNewMeshContainer->vMax);
	// mesh�� FVF�� ���Ͽ� POSITION���� ���� �ٿ�� �ڽ��� �����ϱ� ���� min���� max���� �����Ѵ�.

	pMesh->UnlockVertexBuffer();

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL_DERIVED[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMaterials, sizeof(D3DXMATERIAL_DERIVED) * pNewMeshContainer->NumMaterials); // ����

	pNewMeshContainer->pMeshTexture = new MESHTEXTURE[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMeshTexture, sizeof(MESHTEXTURE) * pNewMeshContainer->NumMaterials);

	if (0 != NumMaterials)
	{
		// ������ �ؽ��� ���� 
		for (size_t i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			pNewMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;   // ������ ����
			pNewMeshContainer->pMaterials[i].pTextureFilename = pMaterials[i].pTextureFilename;  // �����̸��� �����Ѵ�.

			_tchar		szFullPath[128] = L"";
			_tchar		szFileName[128] = L"";

			lstrcpy(szFullPath, m_pPath);					// ������ ���� ��θ� �����Ѵ�

			// static mesh�� ���� ���� ���İ��� �� mesh�� ��쿡 ���� ó���Ѵ�.
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
	// mesh FVF�� �����ϴ� �Լ�, ���� 1: Mesh ���� �ɼ� / ���� 2 : Mesh�� FVF / ���� 3 : ��ġ / ���� 4 : ������ Mesh�� �ּ�

	pNewMeshContainer->pSkinInfo = pSkinInfo;   // ���� �Ű������� �־��ش�.
	pNewMeshContainer->pSkinInfo->AddRef();     

	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones();  // Mesh�� ������ ��ġ�� �ִ� ���� ������ �޴´�

	pNewMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pNewMeshContainer->dwNumBones]; 
	// ���������� ���� ��������� �ּҸ� �����ϱ� ���Ͽ� �����Ҵ��� �Ѵ�
	pNewMeshContainer->pOffsetMatrices = new _matrix[pNewMeshContainer->dwNumBones];   
	// ������ ������ �������� �󸶸�ŭ �����̴����� ���� ����� ������ �����ϱ� ���Ͽ� �����Ҵ� 
	pNewMeshContainer->pRenderingMatrices = new _matrix[pNewMeshContainer->dwNumBones];
	// Render �ϱ� ���� ������ �����ϱ� ���Ͽ� �����Ҵ�

	for (size_t i = 0; i < pNewMeshContainer->dwNumBones; i++)
	{
		pNewMeshContainer->pOffsetMatrices[i] = *(pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
		// ������ offsetMatrices�������� �����Ѵ�.
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
