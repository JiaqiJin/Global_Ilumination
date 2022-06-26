#include "../pch.h"
#include "MeshVoxelizer.h"
#include "../DX12/DescriptorHeap.h"

MeshVoxelizer::MeshVoxelizer(ID3D12Device* _device, UINT _x, UINT _y, UINT _z)
	: device(_device), mX(_x), mY(_y), mZ(_z)
{
	mNumDescriptors = 0;
	mViewPort = { 0.0f, 0.0f, (float)mX, (float)mY, 0.0f, 1.0f };
	mScissorRect = { 0,0,(int)mX, (int)mY };
}


void MeshVoxelizer::InitVoxelizer()
{

}

void MeshVoxelizer::OnResize(UINT newX, UINT newY, UINT newZ)
{

}

D3D12_VIEWPORT MeshVoxelizer::Viewport() const
{
	return mViewPort;
}

D3D12_RECT MeshVoxelizer::ScissorRect() const
{
	return mScissorRect;
}
