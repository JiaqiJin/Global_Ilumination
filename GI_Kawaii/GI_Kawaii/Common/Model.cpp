#include "../pch.h"
#include "Model.h"

Model::Model()
{

}

Model::Model(ModelType _type, std::string _matName)
    : modelType(_type)
{

}

void Model::InitModel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    switch (modelType)
    {
    case TEMPLATE_MODEL:
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

    CreateBuffers(device);
    UploadBuffers(device, cmdList);
}

void Model::CreateBuffers(ID3D12Device* device)
{
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),// this is a default heap
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

void Model::UploadBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
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

    D3D12_SUBRESOURCE_DATA subResourceDataVertex = {};
    subResourceDataVertex.pData = static_cast<void*>(vertices.data());
    subResourceDataVertex.RowPitch = VertexBufferByteSize;
    subResourceDataVertex.SlicePitch = subResourceDataVertex.RowPitch;

    D3D12_SUBRESOURCE_DATA subResourceDataIndex = {};
    subResourceDataIndex.pData = static_cast<void*>(indices.data());
    subResourceDataIndex.RowPitch = IndexBufferByteSize;
    subResourceDataIndex.SlicePitch = subResourceDataIndex.RowPitch;

    UpdateSubresources<1>(cmdList, VertexBufferGPU.Get(), VertexBufferUploader.Get(), 0, 0, 1, &subResourceDataVertex);
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(VertexBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ));

    UpdateSubresources<1>(cmdList, IndexBufferGPU.Get(), IndexBufferUploader.Get(), 0, 0, 1, &subResourceDataIndex);
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(IndexBufferGPU.Get(), 
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

D3D12_VERTEX_BUFFER_VIEW Model::getVertexBufferView() const
{
    return vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Model::getIndexBufferView() const
{
    return indexBufferView;
}

std::unordered_map<std::string, SubMeshGeometry>& Model::getDrawArgs()
{
    return DrawArgs;
}

void Model::buildGeometry()
{

}

void Model::buildQuadGeometry()
{
    Geometry mGeo;
    Geometry::MeshData quad = mGeo.CreateQuad(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

    SubMeshGeometry quadSubMesh;
    quadSubMesh.IndexCount = (UINT)quad.Indices32.size();
    quadSubMesh.StartIndexLocation = 0;
    quadSubMesh.BaseVertexLocation = 0;
    DrawArgs["quad"] = quadSubMesh;

    auto totalVertCount = quad.Vertices.size();
    vertices.resize(totalVertCount);

    UINT k = 0;
    for (size_t i = 0; i < quad.Vertices.size(); ++i, ++k) 
    {
        vertices[k].Pos = quad.Vertices[i].Position;
        vertices[k].Normal = quad.Vertices[i].Normal;
        vertices[k].TexC = quad.Vertices[i].TexC;
    }

    indices.insert(indices.end(), std::begin(quad.GetIndices16()), std::end(quad.GetIndices16()));
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
    CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
    CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    VertexBufferByteSize = vbByteSize;
    VertexByteStride = sizeof(Vertex);
    IndexFormat = DXGI_FORMAT_R32_UINT;
    IndexBufferByteSize = ibByteSize;
}

void Model::buildGeometryAssimp()
{

}

void Model::buildSphereGeometry()
{
    Geometry mGeo;
    Geometry::MeshData sphere = mGeo.CreateSphere(8.f, 20, 20);

    UINT sphereVertexOffset = 0;
    UINT sphereIndexOffset = 0;

    SubMeshGeometry sphereSubMesh;
    sphereSubMesh.IndexCount = (UINT)sphere.Indices32.size();
    sphereSubMesh.StartIndexLocation = sphereIndexOffset;
    sphereSubMesh.BaseVertexLocation = sphereVertexOffset;
    DrawArgs["sphere"] = sphereSubMesh;

    auto totalVertCount = sphere.Vertices.size();
    vertices.resize(totalVertCount);

    UINT k = 0;
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) 
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].TexC = sphere.Vertices[i].TexC;
    }

    indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
    CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
    CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    VertexBufferByteSize = vbByteSize;
    VertexByteStride = sizeof(Vertex);
    IndexFormat = DXGI_FORMAT_R32_UINT;
    IndexBufferByteSize = ibByteSize;
}

void Model::buildCubeGeometry()
{
    Geometry mGeo;
    Geometry::MeshData Cube = mGeo.CreateBox(2.f, 2.f, 2.f, 3.0);

    UINT sphereVertexOffset = 0;
    UINT sphereIndexOffset = 0;

    SubMeshGeometry sphereSubMesh;
    sphereSubMesh.IndexCount = (UINT)Cube.Indices32.size();
    sphereSubMesh.StartIndexLocation = sphereIndexOffset;
    sphereSubMesh.BaseVertexLocation = sphereVertexOffset;
    DrawArgs["cube"] = sphereSubMesh;

    auto totalVertCount = Cube.Vertices.size();
    vertices.resize(totalVertCount);

    UINT k = 0;
    for (size_t i = 0; i < Cube.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = Cube.Vertices[i].Position;
        vertices[k].Normal = Cube.Vertices[i].Normal;
        vertices[k].TexC = Cube.Vertices[i].TexC;
    }

    indices.insert(indices.end(), std::begin(Cube.GetIndices16()), std::end(Cube.GetIndices16()));

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
    CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
    CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    VertexBufferByteSize = vbByteSize;
    VertexByteStride = sizeof(Vertex);
    IndexFormat = DXGI_FORMAT_R32_UINT;
    IndexBufferByteSize = ibByteSize;
}

void Model::buildGridGeometry()
{
    Geometry mGeo;
    Geometry::MeshData sphere = mGeo.CreateGrid(20.f, 20.f, 5.f, 5.0);

    UINT sphereVertexOffset = 0;
    UINT sphereIndexOffset = 0;

    SubMeshGeometry sphereSubMesh;
    sphereSubMesh.IndexCount = (UINT)sphere.Indices32.size();
    sphereSubMesh.StartIndexLocation = sphereIndexOffset;
    sphereSubMesh.BaseVertexLocation = sphereVertexOffset;
    DrawArgs["grid"] = sphereSubMesh;

    auto totalVertCount = sphere.Vertices.size();
    vertices.resize(totalVertCount);

    UINT k = 0;
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].TexC = sphere.Vertices[i].TexC;
    }

    indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
    CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
    CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    VertexBufferByteSize = vbByteSize;
    VertexByteStride = sizeof(Vertex);
    IndexFormat = DXGI_FORMAT_R32_UINT;
    IndexBufferByteSize = ibByteSize;
}

void Model::buildCylinderGeometry()
{
    Geometry mGeo;
    Geometry::MeshData sphere = mGeo.CreateCylinder(5.f, 5.f, 5.f, 5.0, 5.0);

    UINT sphereVertexOffset = 0;
    UINT sphereIndexOffset = 0;

    SubMeshGeometry sphereSubMesh;
    sphereSubMesh.IndexCount = (UINT)sphere.Indices32.size();
    sphereSubMesh.StartIndexLocation = sphereIndexOffset;
    sphereSubMesh.BaseVertexLocation = sphereVertexOffset;

    DrawArgs["grid"] = sphereSubMesh;

    auto totalVertCount = sphere.Vertices.size();
    vertices.resize(totalVertCount);

    UINT k = 0;
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].TexC = sphere.Vertices[i].TexC;
    }

    indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

    ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
    CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
    CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    VertexBufferByteSize = vbByteSize;
    VertexByteStride = sizeof(Vertex);
    IndexFormat = DXGI_FORMAT_R32_UINT;
    IndexBufferByteSize = ibByteSize;
}