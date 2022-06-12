#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Material.h"
#include "Geometry.h"

class CommandContext;

enum ModelType
{
	DEFAULT_MODEL = 0, 
	ASSIMP_MODEL = 1,
	QUAD_MODEL = 2, 
	GRID_MODEL = 3, 
	CUBE_MODEL = 4, 
	SPHERE_MODEL = 5, 
	CYLINDER_MODEL = 6 
};

struct Mesh
{
	std::string materialName;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	d3dUtil::Bound Bounds;
};

class Model
{
public:
	Model();
	Model(ModelType type, std::string matName);

	Model(const Model& rhs) = delete;
	Model& operator=(const Model& rhs) = delete;


	void CreateBuffer(ID3D12Device* device);
	void CreateUploadBuffer(ID3D12Device* device, CommandContext* cmdObj);

	void InitModel(ID3D12Device* device, CommandContext* cmdObj);

	void appendAssimpMesh(const aiScene* aiscene, aiMesh* aimesh);

	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const;
	std::unordered_map<std::string, Mesh>& getDrawArgs();
	const d3dUtil::Bound& getBounds() const;

	void setWorldMatrix(const DirectX::XMFLOAT4X4& mat);
	DirectX::XMFLOAT4X4& getWorldMatrix();
	float getObj2VoxelScale();
	void setObj2VoxelScale(float _scale);


public:
	std::string Name;

	std::string mMatName;

	ModelType modelType = ModelType::DEFAULT_MODEL;

	bool IsDynamic;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, Mesh> DrawArgs;

protected:
	void buildGeometry();
	void buildQuadGeometry();
	void buildGeometryAssimp();

	void buildSphereGeometry();
	void buildCubeGeometry();
	void buildGridGeometry();
	void buildCylinderGeometry();

protected:

	UINT globalMeshID;
	std::string MatName;

	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices; // specifically for large meshes whoes index size > 65535

	DirectX::XMFLOAT4X4 World = MathUtils::Identity4x4();
	DirectX::XMFLOAT4X4 texTransform = MathUtils::Identity4x4();
	float Obj2VoxelScale = 1.0f;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately. 
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader;

	// buffer info
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	d3dUtil::Bound mBounds;
};