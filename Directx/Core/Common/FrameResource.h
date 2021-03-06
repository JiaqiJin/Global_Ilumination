#ifndef FRAME_RESOURCE_H
#define FRAME_RESOURCE_H

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 TexTransform = MathUtils::Identity4x4();
    float Obj2VoxelScale = 1; // used to scale model down to screen space
    int IsDynamic = false;
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 ShadowTransform = MathUtils::Identity4x4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f; // add padding since CB read by float4 and float4
    DirectX::XMFLOAT3 LightPosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad2 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
    DirectX::XMFLOAT4X4 VoxelView = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 VoxelProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 VoxelViewProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT3 camLookDir = { 0.f,0.f,0.f };
    float cbPerObjectPad3 = 0.0f;
    DirectX::XMFLOAT3 camUpDir = { 0.f,0.f,0.f };
    int showVoxel = 1;
    int showDirect = 0;

};

class FrameResource
{
public:

    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT radianceCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    ~FrameResource();

public:
    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
  
    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 Fence = 0;

private:

    UINT objCBByteSize;
    UINT matCBByteSize;
};

#endif // !FRAME_RESOURCE_H

