#pragma once

#include "Engine_Defines.h"
#include "Base.h"


// �Է���ġ�� ���� �ʱ�ȭ�۾� �� ����.
// ���� Ű���¿����� ������ �����Ѵ�.

BEGIN(Engine)

class DLL_EXPORT CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	enum DIM {DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON, DIM_XBUTTON};
	enum DIMM {DIMM_X, DIMM_Y, DIMM_Z};
private:
	explicit CInput_Device();
	virtual ~CInput_Device();
public:
	_byte Get_DIKeyState(_ubyte byDIKey) {
		return m_KeyState[byDIKey]; }

	_byte Get_DIMouseState(DIM eDIMouse) {
		return m_MouseState.rgbButtons[eDIMouse];}

	_long Get_DIMouseMove(DIMM eDIMouse) {
		return *(&m_MouseState.lX + eDIMouse); }
	
public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void Check_InputState(void);

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr; // �Է���ġ�� ���� ����, �Է���ü�� �����ϱ����� ��ü�� ����.
private:
	LPDIRECTINPUTDEVICE8		m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;
private:
	_byte						m_KeyState[256] = { 0 };
	DIMOUSESTATE				m_MouseState;

private:
	HRESULT Ready_KeyBoard_Device(HWND hWnd);
	HRESULT Ready_Mouse_Device(HWND hWnd);
private:
	virtual _ulong Free(void);
};

END