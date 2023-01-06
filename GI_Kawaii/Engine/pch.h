#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <comdef.h> // For _com_error class (used to decode HR result codes).

#include <fcntl.h> 
#include <Shlwapi.h> 

#pragma comment( lib, "Shlwapi.lib" )

// STL
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>

namespace fs = std::filesystem;

// Common lock type
using scoped_lock = std::lock_guard<std::mutex>;