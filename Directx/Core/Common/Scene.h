#pragma once

#include "Material.h"
#include "Model.h"
#include "Camera.h"
#include "Texture.h"

class CommandContext;

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

	UINT ObjCBIndex = -1;

	std::vector<std::unique_ptr<MeshInfo>> mMeshInfos;

	d3dUtil::Bound mBound;

	float Obj2VoxelScale;

	// tags for object types
	bool IsDynamic = false;

};

class Scene
{
public:
	Scene();
	Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight);

	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;

	void initScene();
	void resize(const int& width, const int& height);

	std::unordered_map<std::string, std::unique_ptr<Material>>& getMaterialMap();
	std::unordered_map<std::string, std::unique_ptr<Texture>>& getTexturesMap();
	std::unordered_map<std::string, std::unique_ptr<Model>>& getModelsMap();
	std::unordered_map<std::string, std::unique_ptr<Camera>>& getCamerasMap();
	std::unordered_map<std::string, std::unique_ptr<ObjectInfo>>& getObjectInfos();
	const std::vector<std::vector<ObjectInfo*>>& getObjectInfoLayer();

private:
	void buildMaterials();
	void loadTextures();
	void loadModels();
	void populateMeshInfos();
	void buildCameras();

	void loadAssetFromAssimp(const std::string filepath);
	void processNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel);
private:
	UINT mClientWidth, mClientHeight;

	std::shared_ptr<CommandContext> cpyCommandContext;
	ID3D12Device* md3dDevice;

	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Model>> mModels;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<Camera>> mCameras;

	std::unordered_map<std::string, std::unique_ptr<ObjectInfo>> mObjectInfos;
	std::vector<std::vector<ObjectInfo*>> mObjectInfoLayer;

	UINT globalTextureSRVDescriptorHeapIndex = 0;
	UINT globalMatCBindex = 0;
};