#pragma once

#include "Common/Core.h"
#include "Utility/MathUtils.h"
#include "Utility/Utils.h"
#include "Common/Model.h"
#include "Common/FrameResource.h"
#include "Common/Scene.h"
#include "Scene/ShadowMap.h"
#include "Common/Camera.h"
#include "Scene/MeshVoxelizer.h"
#include "RHI/DX12/DX12_DescriptorHeap.h"
#include "Common/Texture.h"
#include "Scene/DeferredRenderer.h"
#include "Scene/DeferredRenderer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class App : public Core
{
public:
    App(HINSTANCE hInstance);
    App(const App& rhs) = delete;
    App& operator=(const App& rhs) = delete;
    ~App();

    virtual bool Initialize() override;

private:
    virtual void CreateRtvAndDsvDescriptorHeaps() override;
    virtual void OnResize()override;
    virtual void Update(const Timer& gt)override;
    virtual void Draw(const Timer& gt)override;
    virtual void OnDestroy() override;

    virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

    void OnKeyboardInput(const Timer& gt);
    void UpdateCBs(const Timer& gt);
    void UpdateObjectCBs(const Timer& gt);
    void UpdateMainPassCB(const Timer& gt);
    void UpdateMaterialCBs(const Timer& gt);
    void UpdateRadiancePassCB(const Timer& gt);
    void UpdateCamera(const Timer& gt);
    void UpdateShadowPassCB(const Timer& gt);
    void UpdateScenePhysics(const Timer& gt);

    void BuildRootSignature();
    void BuildDescriptorHeaps();
    void BuildShadersAndInputLayout();
    void BuildPSOs();
    void BuildFrameResources();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<ObjectInfo*>& objInfos);
    void DrawScene2ShadowMap();
    void VoxelizeMesh();
    void DrawScene2GBuffers();
    void DrawScene();

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

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
private:
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    std::unique_ptr<Scene> mScene;
    std::unique_ptr<ShadowMap> mShadowMap;
    std::unique_ptr<DeferredRenderer> mDeferredRenderer;
    std::unique_ptr<MeshVoxelizer> mMeshVoxelizer;

    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
    std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> mRootSignatures;
    std::unordered_map<std::string, std::unique_ptr<DX12_DescriptorHeap>> mSrvHeaps;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    UINT NumRTVs;
    UINT NumDSVs;
    UINT NumSRVs;

    //common camera
    XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 mView = MathUtils::Identity4x4();
    XMFLOAT4X4 mProj = MathUtils::Identity4x4();
    float mTheta = 1.5f * XM_PI;
    float mPhi = 0.2f * XM_PI;
    float mRadius = 85.0f;


    POINT mLastMousePos;

    bool mIsWireframe = false;
    bool mShowVoxel = false;
};