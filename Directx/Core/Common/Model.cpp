#include "../pch.h"
#include "Model.h"

Model::Model()
{

}

Model::Model(ModelType type)
	: modelType(type)
{

}

void Model::InitModel(ID3D12Device* device)
{
    switch (modelType)
    {
    case DEFAULT_MODEL:
        buildGeometry();
        break;
    case ASSIMP_MODEL:
        buildGeometryAssimp();
        break;
    case QUAD_MODEL:
        buildQuadGeometry();
        break;
    case GRID_MODEL:
        buildGridGeometry();
        break;
    case CUBE_MODEL:
        buildCubeGeometry();
        break;
    case SPHERE_MODEL:
        buildSphereGeometry();
        break;
    case CYLINDER_MODEL:
        buildCylinderGeometry();
        break;
    default:
        buildGeometry();
        break;
    }

    CreateBuffer(device);
    CreateUploadBuffer(device);
}

D3D12_VERTEX_BUFFER_VIEW Model::getVertexBufferView() const
{
	return vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Model::getIndexBufferView() const
{
	return indexBufferView;
}

std::unordered_map<std::string, Mesh>& Model::getDrawArgs()
{
	return DrawArgs;
}

void Model::CreateBuffer(ID3D12Device* device)
{
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // this is a default heap
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(VertexBufferByteSize),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&VertexBufferGPU)
    ));

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(IndexBufferByteSize),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&IndexBufferGPU)
    ));

    vertexBufferView.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = VertexByteStride;
    vertexBufferView.SizeInBytes = VertexBufferByteSize;

    indexBufferView.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
    indexBufferView.Format = IndexFormat;
    indexBufferView.SizeInBytes = IndexBufferByteSize;
}

void Model::CreateUploadBuffer(ID3D12Device* device)
{
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(VertexBufferByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(VertexBufferUploader.GetAddressOf())
    ));

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(IndexBufferByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(IndexBufferUploader.GetAddressOf())
    ));
}

void Model::buildGeometry()
{

}

void Model::buildQuadGeometry()
{

}

void Model::buildGeometryAssimp()
{

}

void Model::buildSphereGeometry()
{

}
void Model::buildCubeGeometry()
{

}

void Model::buildGridGeometry()
{

}

void Model::buildCylinderGeometry()
{

}