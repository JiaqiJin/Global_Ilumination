#pragma once

#include "../RHI/DX12/DX12_DescriptorHeap.h"

struct MeshVoxelData
{
	DirectX::XMFLOAT4X4 mVoxelView = MathUtils::Identity4x4();
	DirectX::XMFLOAT4X4 mVoxelProj = MathUtils::Identity4x4();
};

enum class VOXEL_TEXTURE_TYPE
{
	ALBEDO = 0, NORMAL, EMISSIVE, RADIANCE, FLAG, COUNT
};

class VoxelizerTexture
{
public:
	VoxelizerTexture(ID3D12Device* _device, UINT _x, UINT _y, UINT _z);
	VoxelizerTexture(const VoxelizerTexture& rhs) = delete;
	VoxelizerTexture& operator=(const VoxelizerTexture& rhs) = delete;
	~VoxelizerTexture() = default;

	ID3D12Resource* GetResourcePtr() { return m3DTexture.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle4UAV() const { return mhCPUuav; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle4UAV() const { return mhGPUuav; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle4SRV() const { return mhCPUsrv; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle4SRV() const { return mhGPUsrv; }
	UINT GetNumDescriptors() { return mNumDescriptors; }
	D3D12_VIEWPORT Viewport() const { return mViewPort; }
	D3D12_RECT ScissorRect() const { return mScissorRect; }

	void SetupUAVCPUGPUDescOffsets(
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUUav,
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUUav);
	void SetupSRVCPUGPUDescOffsets(
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUSrv,
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUSrv);

	void Init();
	void OnResize(UINT newX, UINT newY, UINT newZ);

private:
	void BuildUAVDescriptors();
	void BuildSRVDescriptors();
	void BuildResources();

private:
	ID3D12Device* device;
	UINT mX, mY, mZ;
	UINT mNumDescriptors;

	Microsoft::WRL::ComPtr<ID3D12Resource> m3DTexture;

	D3D12_CPU_DESCRIPTOR_HANDLE mhCPUuav;
	D3D12_GPU_DESCRIPTOR_HANDLE mhGPUuav;
	D3D12_CPU_DESCRIPTOR_HANDLE mhCPUsrv;
	D3D12_GPU_DESCRIPTOR_HANDLE mhGPUsrv;

	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	DXGI_FORMAT mSRVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mUAVFormat = DXGI_FORMAT_R32_UINT;
	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;
};

class MeshVoxelizer
{
public:
	// used a voxel grid with 256x256x256 voxels
	MeshVoxelizer(ID3D12Device* _device, UINT _x, UINT _y, UINT _z);
	MeshVoxelizer(const MeshVoxelizer& rhs) = delete;
	MeshVoxelizer& operator=(const MeshVoxelizer& rhs) = delete;
	~MeshVoxelizer() = default;

	void InitVoxelizer();
	void OnResize(UINT newX, UINT newY, UINT newZ);

	void SetDescriptor4Voxel(DX12_DescriptorHeap* descHeap);

	MeshVoxelData& GetUniformData() { return mData; }
	D3D12_VIEWPORT Viewport() const { return mViewPort; }
	D3D12_RECT ScissorRect() const { return mScissorRect; }

	VoxelizerTexture* GetVoxelTexture(VOXEL_TEXTURE_TYPE _type) { return mVoxelTexture[_type].get(); }
	std::unordered_map<VOXEL_TEXTURE_TYPE, std::unique_ptr<VoxelizerTexture>>& GetVoxelTexturesMap() { return mVoxelTexture; }

private:
	void PopulateUniformData();

	ID3D12Device* device;

	UINT mX, mY, mZ;
	UINT mNumDescriptors;

	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;

	MeshVoxelData mData;
	std::unordered_map<VOXEL_TEXTURE_TYPE, std::unique_ptr<VoxelizerTexture>> mVoxelTexture;
};