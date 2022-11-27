#pragma once

#include "Model.h"

struct MeshInfo
{
	MeshInfo() = default;

	DirectX::XMFLOAT4X4 World = MathUtils::Identity4x4();

	Model* Model = nullptr;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

struct ObjectInfo
{
	ObjectInfo() = default;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

	std::vector<std::unique_ptr<MeshInfo>> mMeshInfos;

	UINT ObjCBIndex = -1;
};

class Scene
{
public:
	Scene();
	Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight);

	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;

	void InitScene();
	void Resize(const int width, const int height);

private:
	void LoadModels();

	void LoadAssetFromAssimp(const std::string filepath);
	void ProcessNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel);
private:

	ID3D12Device* md3dDevice;
	UINT mClientWidth, mClientHeight;

	std::unordered_map<std::string, std::unique_ptr<Model>> mModels;
	std::unordered_map<std::string, std::unique_ptr<ObjectInfo>> mObjectInfos;
};