#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// �������� �����Ѵ�.
// ������ �������� �־�α� ���� �޸𸮵��(����)�� �����.

class DLL_EXPORT CRectUI_Texture final : public CVIBuffer
{
private:
	explicit CRectUI_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRectUI_Texture(const CRectUI_Texture& rhs);
	virtual ~CRectUI_Texture() = default;
public:
	HRESULT Ready_Buffer(void);
	void Render_Buffer(void);
	void Set_Pos_Buffer(const _float& fWinSizeX, const _float& fWinSizeY, const _float& fSizeX, const _float& fSizeY);
public:
	virtual CComponent* Clone(void) final;
public:
	static CRectUI_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free(void) final;

};

END

