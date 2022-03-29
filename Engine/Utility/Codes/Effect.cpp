#include "..\Headers\Effect.h"

USING(Engine)

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CComponent(rhs)
{

}

HRESULT CEffect::Ready_Effect(const _tchar * pFilePath)
{
	return NOERROR;
}

_ulong CEffect::Free(void)
{
	return CComponent::Free();
}
