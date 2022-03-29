#include "..\Headers\Stack.h"


USING(Engine)

CStack::CStack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{

}


HRESULT CStack::Ready_Stack(void)
{
	m_iArr = -1;

	ZeroMemory(&m_ArrStack, sizeof(_int) * MAX_SIZE);

	return NOERROR;
}

void CStack::Setting_MaxSize(_int iMax)
{
	m_ArrStack = new _int[iMax];
}

CComponent * CStack::Clone(void)
{
	return new CStack(*this);
}

CStack * CStack::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStack *		pInstance = new CStack(pGraphicDev);

	if (FAILED(pInstance->Ready_Stack()))
	{
		MSG_BOX("CStack Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CStack::Free(void)
{
	Safe_Delete_Array(m_ArrStack);


	return CComponent::Free();
}
