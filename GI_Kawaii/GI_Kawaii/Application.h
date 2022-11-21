#pragma once

#include "Common/Core.h"
#include "Utility/MathUtils.h"
#include "Utility/Utils.h"
#include "Common/Model.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathUtils::Identity4x4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
};

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = MathUtils::Identity4x4();
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
    void BuildBoxGeometry();

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
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
    Model* TestModel;

    PassConstants mMainPassCB;
};