#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Material.h"
#include "Geometry.h"

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
	Model(ModelType type);

	Model(const Model& rhs) = delete;
	Model& operator=(const Model& rhs) = delete;

	void InitModel(ID3D12Device* device);

	void CreateBuffer(ID3D12Device* device);
	void CreateUploadBuffer(ID3D12Device* device);

	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const;
	std::unordered_map<std::string, Mesh>& getDrawArgs();
public:
	std::string Name;

	ModelType modelType = ModelType::DEFAULT_MODEL;

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