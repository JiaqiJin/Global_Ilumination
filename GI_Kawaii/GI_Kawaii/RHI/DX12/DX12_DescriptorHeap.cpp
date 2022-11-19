#include "../../pch.h"
#include "DX12_DescriptorHeap.h"

DX12_DescriptorHeap::DX12_DescriptorHeap()
{
	memset(this, 0, sizeof(*this));
}

bool DX12_DescriptorHeap::CreateDescriptorHeap(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type,
	UINT NumDescriptors, bool bShaderVisible)
{
	mCurrentOffset = 0;
	Desc.Type = Type;
	Desc.NumDescriptors = NumDescriptors;
	Desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	Desc.NodeMask = 0;
	mHandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&m_DescriptorHeap)));

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12_DescriptorHeap::GetCPUHandle(UINT index)
{
	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(index, mHandleIncrementSize);
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12_DescriptorHeap::GetGPUHandle(UINT index)
{
	assert(Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	handle.Offset(index, mHandleIncrementSize);
	return handle;
}