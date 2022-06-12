#pragma once

#include "Common/Application.h"
#include "Common/Geometry.h"
#include "Common/Scene.h"
#include "Common/FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class DescriptorHeap;

class WindowApplication : public Application
{
public:
	WindowApplication(HINSTANCE hInstance);
	~WindowApplication();

	virtual bool Initialize() override;

private:

	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void OnDestroy() override;
    virtual void OnResize() override;

    // input callbacks 
    virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

    // key board input
    virtual void OnKeyDown(WPARAM btnState) override;
    virtual void OnKeyUp(WPARAM btnState) override;
    void OnKeyboardInput(const GameTimer& gt);

    // Update
    void UpdateObjectCBs(const GameTimer& gt);
    void UpdateMainPassCB(const GameTimer& gt);
    void UpdateCBs(const GameTimer& gt);

    void BuildDescriptorHeaps();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildPSO();
    void CreatePSO(ID3D12PipelineState** pso,
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
        ID3DBlob* geometryShader = nullptr);

    void BuildFrameResources();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<ObjectInfo*>& objInfos);

    void DrawScene();

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:

    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    std::unique_ptr<Scene> mScene;
    std::unique_ptr<class DeferredRenderer> mDeferredRenderer;

    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
    std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> mRootSignatures;
    std::unordered_map<std::string, std::unique_ptr<DescriptorHeap>> mSrvHeaps;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    XMFLOAT4X4 mWorld = MathUtils::Identity4x4();
    XMFLOAT4X4 mView = MathUtils::Identity4x4();
    XMFLOAT4X4 mProj = MathUtils::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
    bool isFreeCamEnabled = true;

    UINT NumRTVs;
    UINT NumDSVs;
    UINT NumSRVs;
};