#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class DLL_EXPORT CCube_Texture final : public CVIBuffer
{
private:
	explicit CCube_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCube_Texture(const CCube_Texture& rhs);
	virtual ~CCube_Texture() = default;
public:
	HRESULT		Ready_Buffer();
	void		Render_Buffer();
public:
	virtual CComponent* Clone() final;
public:
	static CCube_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void) final;
};

END