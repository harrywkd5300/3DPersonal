#include "..\Headers\Resources.h"

USING(Engine)

CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CResources::CResources(const CResources & rhs)
	: CComponent(rhs)
{
}

CResources::~CResources()
{
}

_ulong CResources::Free(void)
{
	return CComponent::Free();
}
