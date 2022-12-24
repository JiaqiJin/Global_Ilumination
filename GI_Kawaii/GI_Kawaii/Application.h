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

const int gNumFrameResources = 3;

struct RenderItem
{
    RenderItem() = default;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    XMFLOAT4X4 World = MathUtils::Identity4x4();

    XMFLOAT4X4 TexTransform = MathUtils::Identity4x4();

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify obect data we should set 
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    int NumFramesDirty = gNumFrameResources;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    Material* Mat = nullptr;
    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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
    void UpdateMaterialCB(const Timer& gt);
    void UpdateCamera(const Timer& gt);
    void UpdateShadowPassCB(const Timer& gt);

    void BuildRootSignature();
    void BuildDescriptorHeaps();
    void BuildShadersAndInputLayout();
    void BuildPSOs();
    void BuildFrameResources();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<ObjectInfo*>& objInfos);
    void DrawSceneToShadowMap();
    void VoxelizeMesh();

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

    //// Assimp
   /* void LoadAssetFromAssimp(const std::string filepath);
    void processNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel);*/
private:
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    UINT mCbvSrvDescriptorSize = 0;
    UINT mShadowMapHeapIndex = 0;
    UINT mSkyTexHeapIndex = 0;

    UINT mNullCubeSrvIndex = 0;
    UINT mNullTexSrvIndex = 0;

    ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<std::string, std::unique_ptr<L_Texture>> mTextures;
    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
    std::unordered_map<std::string, std::unique_ptr<DX12_DescriptorHeap>> mSrvHeaps;
    std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> mRootSignatures;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    // List of all the render items.
    std::vector<std::unique_ptr<RenderItem>> mAllRitems;

    // Render items divided by PSO.
    std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

    Camera mCamera;

    PassConstants mMainPassCB;
    PassConstants mShadowPassCB;

    XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 mView = MathUtils::Identity4x4();
    XMFLOAT4X4 mProj = MathUtils::Identity4x4();
    float mTheta = 1.5f * XM_PI;
    float mPhi = 0.2f * XM_PI;
    float mRadius = 15.0f;

    POINT mLastMousePos;

    // ShadowMap
    std::unique_ptr<ShadowMap> mShadowMap;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;
    DirectX::BoundingSphere mSceneBounds;

    // Mesh Voxelizer
    std::unique_ptr<MeshVoxelizer> mMeshVoxelizer;

    // Deffered
    std::unique_ptr<DeferredRenderer> mDefferedRenderer;

    // Scene
    std::unique_ptr<Scene> mScene;

    bool mIsWireframe = false;
};