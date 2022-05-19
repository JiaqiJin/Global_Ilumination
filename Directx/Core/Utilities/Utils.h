#pragma once

class d3dUtil
{
public:

    static bool IsKeyDown(int vkeyCode);

    static std::string ToString(HRESULT hr);

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);

    static D3D12_SHADER_BYTECODE getShaderBytecode(ID3DBlob* blob);

    static int GetNumMipmaps(int w, int h, int d);

    static int gNumFrameResources;

    // ideal layout for descriptor tables (CBVs) directly inside of rootsignatures from top to 
    // bottom (hight freq to low freq) : perDrawCall(VS) -> PerDrawCall(PS) -> perMesh(VS) -> perMesh(PS)
    // -> perMaterial -> perFrame(VS) -> perFrame(PS)
    enum MAIN_PASS_UNIFORM {
        OBJ_CBV,                // per object
        MATERIAL_CBV,           // per mesh
        MAINPASS_CBV,           // per render pass
        DIFFUSE_TEX_TABLE,
        SHADOWMAP_TEX_TABLE,
        G_BUFFER, // pos, albedo, normal, depth
        VOXEL,
        RADIANCEMIP,
        COUNT
    };

    struct Bound {
        DirectX::XMFLOAT3 maxBound;
        DirectX::XMFLOAT3 minBound;
        Bound() {
            maxBound = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            minBound = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        }
        void updateBounds(DirectX::XMFLOAT3 pos) {
            maxBound.x = maxBound.x > pos.x ? maxBound.x : pos.x;
            maxBound.y = maxBound.y > pos.y ? maxBound.y : pos.y;
            maxBound.z = maxBound.z > pos.z ? maxBound.z : pos.z;

            minBound.x = minBound.x < pos.x ? minBound.x : pos.x;
            minBound.y = minBound.y < pos.y ? minBound.y : pos.y;
            minBound.z = minBound.z < pos.z ? minBound.z : pos.z;
        }
    };
};

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}