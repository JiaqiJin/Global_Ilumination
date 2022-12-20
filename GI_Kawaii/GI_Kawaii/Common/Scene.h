#pragma once

#include "Model.h"
#include "Camera.h"
#include "FrameResource.h"
#include "Texture.h"

enum class RenderLayer : int
{
	Default = 0,
	Debug,
	Gbuffer,
	Sky,
	Dynamic,
	Static,
	Count
};


struct MeshInfo
{
	MeshInfo() = default;

	DirectX::XMFLOAT4X4 World = MathUtils::Identity4x4();
	DirectX::XMFLOAT4X4 texTransform = MathUtils::Identity4x4();

	std::string Mat;
	Model* Model = nullptr;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

struct ObjectInfo
{
	ObjectInfo() = default;

	DirectX::XMFLOAT4X4 World = MathUtils::Identity4x4();
	DirectX::XMFLOAT4X4 texTransform = MathUtils::Identity4x4();

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

	Model* Model = nullptr;

	int NumFramesDirty = d3dUtil::gNumFrameResources;

	std::vector<std::unique_ptr<MeshInfo>> mMeshInfos;

	UINT ObjCBIndex = -1;

	d3dUtil::Bound mBound;
};

class Scene
{
public:
	Scene();
	Scene(ID3D12Device* _device, ID3D12GraphicsCommandList* cmdList, UINT _mClientWidth, UINT _mClientHeight);

	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;

	void InitScene();
	void Resize(const int width, const int height);

	std::unordered_map<std::string, std::unique_ptr<Material>>& getMaterialMap();
	std::unordered_map<std::string, std::unique_ptr<Texture>>& getTexturesMap();
	std::unordered_map<std::string, std::unique_ptr<Model>>& getModelsMap();
	std::unordered_map<std::string, std::unique_ptr<Camera>>& getCamerasMap();
	std::unordered_map<std::string, std::unique_ptr<ObjectInfo>>& getObjectInfos();
private:
	void BuildMaterials();
	void LoadTextures();
	void LoadModels();
	void PopulateMeshInfos();
	void BuildCameras();

	void LoadAssetFromAssimp(const std::string filepath);
	void ProcessNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel);
private:

	ID3D12Device* md3dDevice;
	ID3D12GraphicsCommandList* cmdList;
	UINT mClientWidth, mClientHeight;

	std::unordered_map<std::string, std::unique_ptr<Model>> mModels;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Camera>> mCameras;
	std::unordered_map<std::string, std::unique_ptr<ObjectInfo>> mObjectInfos;

	UINT globalTextureSRVDescriptorHeapIndex = 0;
	UINT globalMatCBindex = 0;
};