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
        App LocalApp(hInstance);
        if (!LocalApp.Initialize())
            return 0;

        return LocalApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

/*
Initialize
    - Init RootSignature (Describe 2 constant buffer)
    - Shaders and Input Layout (Compiling shader and define Shader input)
    - Build The Geometry
    - Build All render object we need to render in the scene (Model, primitive, index and vertex count etc)
    - Build a frame resource for each frame (All Render objects).
    - Build Descriptor Heap ()
    - Build Constante Buffer (creating constant buffer view for each frame resource object)

    CD3DX12_ROOT_PARAMETER slotRootParameter[5];
    SetGraphicsRootConstantBufferView(rootParameterIndex); -> [0-4];
*/

// https://zhuanlan.zhihu.com/p/414691569
// https://jose-villegas.github.io/post/deferred_voxel_shading/
// https://xeolabs.com/pdfs/OpenGLInsights.pdf