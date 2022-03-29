#pragma once

#ifdef _DEBUG

#pragma comment(linker,"/entry:wWinMainCRTStartup /subsystem:console")

#endif  // _DEBUG  

#include "../Default/stdafx.h"

using namespace std;

#include "Constant.h"
#include <iostream>

#include <io.h>

#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

extern HWND g_hWnd;