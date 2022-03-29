#pragma once

#include "Engine_Defines.h"
#include "Base.h"


// 입력장치에 대한 초기화작업 및 설정.
// 현재 키상태에대한 정보를 보관한다.

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
	LPDIRECTINPUT8				m_pInputSDK = nullptr; // 입력장치에 대한 설정, 입력장체를 제어하기위한 객체를 생성.
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