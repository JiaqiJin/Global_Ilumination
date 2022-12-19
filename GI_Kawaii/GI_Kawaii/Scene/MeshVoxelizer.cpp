#include "../pch.h"
#include "MeshVoxelizer.h"

VoxelizerTexture::VoxelizerTexture(ID3D12Device* _device, UINT _x, UINT _y, UINT _z)
	: device(_device), mX(_x), mY(_y), mZ(_z) 
{
	mNumDescriptors = 2;

	mViewPort = { 0.0f, 0.0f, (float)mX, (float)mY, 0.0f, 1.0f };
	mScissorRect = { 0,0,(int)mX, (int)mY };
}

void VoxelizerTexture::Init()
{
	BuildResources();
}

void VoxelizerTexture::OnResize(UINT newX, UINT newY, UINT newZ)
{
	if ((mX != newX) || (mY != newY) || (mZ != newZ)) {
		mX = newX;
		mY = newY;
		mZ = newZ;
		mViewPort = { 0.0f, 0.0f, (float)mX, (float)mY, 0.0f, 1.0f };
		mScissorRect = { 0,0,(int)mX, (int)mY };
		BuildResources();
		BuildUAVDescriptors();
	}
}

void VoxelizerTexture::BuildUAVDescriptors()
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = mUAVFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = -1;
	device->CreateUnorderedAccessView(m3DTexture.Get(), nullptr, &uavDesc, mhCPUuav);
}

void VoxelizerTexture::BuildSRVDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mSRVFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = 1;
	srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
	device->CreateShaderResourceView(m3DTexture.Get(), &srvDesc, mhCPUsrv);
}

void VoxelizerTexture::BuildResources()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	texDesc.Alignment = 0;
	texDesc.Width = mX;
	texDesc.Height = mY;
	texDesc.DepthOrArraySize = mZ;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(m3DTexture.GetAddressOf())
	));
}

void VoxelizerTexture::SetupUAVCPUGPUDescOffsets(D3D12_CPU_DESCRIPTOR_HANDLE hCPUUav, D3D12_GPU_DESCRIPTOR_HANDLE hGPUUav)
{
	mhCPUuav = hCPUUav;
	mhGPUuav = hGPUUav;
	BuildUAVDescriptors();
}

void VoxelizerTexture::SetupSRVCPUGPUDescOffsets(D3D12_CPU_DESCRIPTOR_HANDLE hCPUSrv, D3D12_GPU_DESCRIPTOR_HANDLE hGPUSrv)
{
	mhCPUsrv = hCPUSrv;
	mhGPUsrv = hGPUSrv;
	BuildSRVDescriptors();
}

// -----------------------------------------

MeshVoxelizer::MeshVoxelizer(ID3D12Device* _device, UINT _x, UINT _y, UINT _z)
	: device(_device), mX(_x), mY(_y), mZ(_z)
{
	mNumDescriptors = 0;
	mViewPort = { 0.0f, 0.0f, (float)mX, (float)mY, 0.0f, 1.0f };
	mScissorRect = { 0,0,(int)mX, (int)mY };

	PopulateUniformData();
}

void MeshVoxelizer::InitVoxelizer()
{
	auto V_Albedo = std::make_unique<VoxelizerTexture>(device, mX, mY, mZ);
	auto V_Normal = std::make_unique<VoxelizerTexture>(device, mX, mY, mZ);
	auto V_Emissive = std::make_unique<VoxelizerTexture>(device, mX, mY, mZ);
	auto V_Radiance = std::make_unique<VoxelizerTexture>(device, mX, mY, mZ);
	auto V_Flag = std::make_unique<VoxelizerTexture>(device, mX, mY, mZ);
	mVoxelTexture[VOXEL_TEXTURE_TYPE::ALBEDO] = std::move(V_Albedo);
	mVoxelTexture[VOXEL_TEXTURE_TYPE::NORMAL] = std::move(V_Normal);
	mVoxelTexture[VOXEL_TEXTURE_TYPE::EMISSIVE] = std::move(V_Emissive);
	mVoxelTexture[VOXEL_TEXTURE_TYPE::RADIANCE] = std::move(V_Radiance);
	mVoxelTexture[VOXEL_TEXTURE_TYPE::FLAG] = std::move(V_Flag);

	for (auto& tex : mVoxelTexture)
	{
		tex.second->Init();
		mNumDescriptors += tex.second->GetNumDescriptors();
	}
}

void MeshVoxelizer::PopulateUniformData()
{

}

void MeshVoxelizer::OnResize(UINT newX, UINT newY, UINT newZ)
{

}
