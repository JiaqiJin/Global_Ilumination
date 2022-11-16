#pragma once

// Off by default warnings
#pragma warning(disable : 4619 4061 4265 4355 4365 4571 4623 4625 4626 4628 4668 4710 4711 4746 4774 4820 4987 5026 5027 5031 5032 5039 5045 5219 26812)

#ifdef __INTEL_COMPILER
#pragma warning(disable : 161 2960 3280)
// warning #161: unrecognized #pragma
// message #2960: allocation may not satisfy the type's alignment; consider using <aligned_new> header
// message #3280: declaration hides member
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma warning(push)
#pragma warning(disable : 4005)
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#pragma warning(pop)

#include <Windows.h>
#include <WindowsX.h>

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

#include <dxgi1_4.h>
#include <d3d12.h>

#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

#include <algorithm>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <unordered_map>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <tchar.h>
#include <cassert>
#include <fstream>
#include <sstream>

#pragma warning(push)
#pragma warning(disable : 5204 5220)
#include <future>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4702)
#include <functional>
#pragma warning(pop)

#include <malloc.h>

#pragma warning(push)
#pragma warning(disable : 4467 5038 5204 5220)
#include <wrl.h>
#pragma warning(pop)

#include <wincodec.h>

// DirectX Tool Kit for Audio is in all versions of DirectXTK12
#include <mmreg.h>
#include <Audioclient.h>

#ifndef XAUDIO2_HELPER_FUNCTIONS
#define XAUDIO2_HELPER_FUNCTIONS
#endif

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>

// 
#include "Utility/Utils.h"
#include "Common/d3dx12.h"