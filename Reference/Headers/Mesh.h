#pragma once

#include "Resources.h"

BEGIN(Engine)

class DLL_EXPORT CMesh abstract : public CResources
{
protected:
	explicit CMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);

public:
	virtual CComponent* Clone(void) = 0;
protected:
	virtual _ulong Free(void) override;
};

END