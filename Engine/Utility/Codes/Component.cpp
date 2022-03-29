#include "..\Headers\Component.h"

USING(Engine)

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_isClone(false)
{
	m_pGraphicDev->AddRef();
}

CComponent::CComponent(const CComponent & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_isClone(true)
{
	m_pGraphicDev->AddRef();
}

CComponent::~CComponent()
{
}

_ulong CComponent::Free(void)
{
	_ulong dwRefCnt = 0;

	Engine::Safe_Release(m_pGraphicDev);

	return dwRefCnt;	
}
