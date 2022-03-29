#include "..\Headers\Material.h"

USING(Engine)

CMaterial::CMaterial(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}

CMaterial::CMaterial(const CMaterial& rhs)
	: CComponent(rhs)
	, m_MaterialInfo(rhs.m_MaterialInfo)
{
}


HRESULT CMaterial::Ready_Material(D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float & fPower)
{
	ZeroMemory(&m_MaterialInfo, sizeof(D3DMATERIAL9));

	m_MaterialInfo.Diffuse  = Diffuse;
	m_MaterialInfo.Ambient  = Ambient;
	m_MaterialInfo.Specular = Specular;
	m_MaterialInfo.Power    = fPower;

	return NOERROR;
}

void CMaterial::SetUp_OnGraphicDev()
{
	m_pGraphicDev->SetMaterial(&m_MaterialInfo);
}

CComponent * CMaterial::Clone()
{
	return new CMaterial(*this);
}

CMaterial * CMaterial::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float & fPower)
{
	CMaterial *		pInstance = new CMaterial(pGraphicDev);

	if (FAILED(pInstance->Ready_Material(Diffuse, Ambient, Specular, fPower)))
	{
		MSG_BOX("CMaterial Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CMaterial::Free()
{
	return CComponent::Free();
}
