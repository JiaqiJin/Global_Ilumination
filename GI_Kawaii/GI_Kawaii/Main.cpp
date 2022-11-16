#include <iostream>
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        Application App(hInstance);
        if (!App.Initialize())
            return 0;

        return App.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

/*
D3D12 ERROR: ID3D12CommandList::ResourceBarrier: 
NULL pointer specified. [ RESOURCE_MANIPULATION ERROR #520: RESOURCE_BARRIER_NULL_POINTER]

Exception thrown at 0x00007FF945964FD9 in GI_Kawaii.exe:
Microsoft C++ exception: _com_error at memory location 0x000000B3A62FE450.

D3D12 ERROR: ID3D12CommandList::ClearRenderTargetView: 
Specified CPU descriptor handle (ptr=0x0000022308D4E360) has not been initialized. 
Render Target View descriptor handles must be initialized before being referenced by Command List API's.
[ EXECUTION ERROR #646: INVALID_DESCRIPTOR_HANDLE]

Exception thrown at 0x00007FF945964FD9 (KernelBase.dll) in GI_Kawaii.exe: 0x0000087D 
(parameters: 0x0000000000000000, 0x000000B3A62FC3A0, 0x000002237D04E920).

Unhandled exception at 0x00007FF945964FD9 (KernelBase.dll) in GI_Kawaii.exe:
0x0000087D (parameters: 0x0000000000000000, 0x000000B3A62FC3A0, 0x000002237D04E920).

*/