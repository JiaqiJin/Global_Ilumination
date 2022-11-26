#pragma once

#include "Common/Core.h"
#include "Utility/MathUtils.h"
#include "Utility/Utils.h"
#include "Common/Model.h"
#include "Common/FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

struct RenderItem
{
    RenderItem() = default;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    XMFLOAT4X4 World = MathUtils::Identity4x4();

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify obect data we should set 
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    int NumFramesDirty = gNumFrameResources;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    Model* RenderModel = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

class Application : public Core
{
public:
    Application(HINSTANCE hInstance);
    Application(const Application& rhs) = delete;
    Application& operator=(const Application& rhs) = delete;
    ~Application();

    virtual bool Initialize() override;

private:
    virtual void CreateRtvAndDsvDescriptorHeaps() override;
    virtual void OnResize() override;
    virtual void Update(const Timer& gt) override;
    virtual void Draw(const Timer& gt) override;
    virtual void OnDestroy() override;

    // input callbacks 
    virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
    // key board input
    virtual void OnKeyDown(WPARAM btnState) override;
    virtual void OnKeyUp(WPARAM btnState) override;

    // per frame updates
    void OnKeyboardInput(const Timer& gt);
    void UpdateCamera(const Timer& gt);
    void UpdateObjectCBs(const Timer& gt);
    void UpdateMainPassCB(const Timer& gt);
    void UpdateMaterialCBs(const Timer& gt);
    void UpdateShadowPassCB(const Timer& gt);
    void UpdateRadiancePassCB(const Timer& gt);
    void UpdateCBs(const Timer& gt);
    void UpdateScenePhysics(const Timer& gt);

    // misc initializations
    void BuildDescriptorHeaps();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildPSOs();
    void CreatePSO(
        ID3D12PipelineState** pso,
        ID3D12RootSignature* rootSignature,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopoType,
        D3D12_BLEND_DESC blendDesc,
        D3D12_RASTERIZER_DESC rasterDesc,
        D3D12_DEPTH_STENCIL_DESC dsState,
        UINT numRenderTargets,
        DXGI_FORMAT renderTargetFormat,
        DXGI_FORMAT depthStencilFormat,
        ID3DBlob* vertexShader,
        ID3DBlob* pixelShader,
        ID3DBlob* geometryShader = nullptr
    );

    void BuildConstantBuffers();
    void BuildSceneGeometry();

    void BuildFrameResources();
    void BuildConstantBufferViews();

    // Render Items
    void BuildRenderItems();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

    // Assimp
    void LoadAssetFromAssimp(const std::string filepath);
    void processNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel);
private:
    // Frame Resource
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

   // std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 mWorld = MathUtils::Identity4x4();
    XMFLOAT4X4 mView = MathUtils::Identity4x4();
    XMFLOAT4X4 mProj = MathUtils::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;

    // Mesh Creation
    Model* TestBoxModel;
    Model* AssimpMode;

    // List of all the render items.
    std::vector<std::unique_ptr<RenderItem>> mAllRitems;

    // Render items divided by PSO.
    std::vector<RenderItem*> mOpaqueRitems;

    PassConstants mMainPassCB;
    UINT mPassCbvOffset = 0;
};