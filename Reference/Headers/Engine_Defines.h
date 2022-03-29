#pragma once

#include <vector>
#include <tuple>
#include <list>
#include <tuple>
#include <map>
#include <algorithm>

#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "dxguid.lib")

#pragma	warning (disable : 4251)

using namespace std;

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Func.h"
#include "Engine_Functor.h"
#include "Engine_Struct.h"

using namespace Engine;