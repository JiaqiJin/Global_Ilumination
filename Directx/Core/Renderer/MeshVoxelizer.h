#pragma once

class DescriptorHeap;

class MeshVoxelizer 
{
public:
	MeshVoxelizer(ID3D12Device* _device, UINT _x, UINT _y, UINT _z);
	MeshVoxelizer(const MeshVoxelizer& rhs) = delete;
	MeshVoxelizer& operator=(const MeshVoxelizer& rhs) = delete;
	~MeshVoxelizer() = default;

	void InitVoxelizer();
	void OnResize(UINT newX, UINT newY, UINT newZ);
	D3D12_VIEWPORT Viewport()const;
	D3D12_RECT ScissorRect()const;
private:
	ID3D12Device* device;
	UINT mX, mY, mZ;
	UINT mNumDescriptors;

	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;
};