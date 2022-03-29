#pragma once

#include "Engine_Defines.h"
#include "Component.h"

#define MAX_SIZE 10

BEGIN(Engine)

class DLL_EXPORT CStack : public CComponent
{
private:
	explicit CStack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStack() = default;

public:
	HRESULT Ready_Stack(void);

public:
	void	Pop();

public:		// Setter
	void	Setting_MaxSize(_int iMax);

private:
	_int	m_ArrStack;
	_int	m_iArr = -1;

public:
	virtual CComponent* Clone(void) final;
	static CStack* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void);
};

