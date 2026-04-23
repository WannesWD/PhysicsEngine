// code for graphics pipeline inspired by https://www.3dgep.com/learning-directx-12-1/#CMake
#define WIND32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <wrl.h>
using namespace Microsoft::WRL;

// Windows min max macros can cause naming conflicts
#if defined(min)
#undef min
#endif // defined(min)


#if defined(max)
#undef max
#endif // defined(max)

#if defined(CreateWindow)
#undef CreateWindow
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <d3dx12.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <memory>

#include <Helpers.h>