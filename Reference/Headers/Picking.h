#pragma once

#include "Component.h"

BEGIN(Engine)

class CVIBuffer;
class CMesh;
class DLL_EXPORT CPicking : public CComponent
{
private:
	explicit CPicking(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPicking(const CPicking& rhs);
	virtual ~CPicking() = default;
private:
	HRESULT		Ready_Picking();

private:
	_vec3		vOutDirection;

public:
	_vec3*		Picking_ToBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);
	_vec3		Picking_ToNaviBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, Engine::CVIBuffer * pTargetBuffer, _ulong uX, _ulong uY, const _matrix* pWorldMatrix = nullptr);

	_bool		Picking_ToMeshBuffer(HWND hWnd, _uint iBackSizeX, _uint iBackSizeY, LPD3DXMESH * pTargetBuffer, _ulong uX, _ulong uY, const _matrix* pWorldMatrix = nullptr);

public:
	virtual CComponent* Clone(void);

public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual _ulong Free();
};

END