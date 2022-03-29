#include "..\Headers\Mesh.h"

USING(Engine)


CMesh::CMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	:CResources(pGraphicDev)
{
}

CMesh::CMesh(const CMesh & rhs)
	:CResources(rhs)
{
}

HRESULT CMesh::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	return NOERROR;
}

_ulong CMesh::Free(void)
{
	return CResources::Free();
}
