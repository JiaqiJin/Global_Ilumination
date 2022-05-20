#pragma once

#include "../pch.h"

class DescriptorHeap
{
public:
	DescriptorHeap() {};

	bool Create(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, bool bShaderVisible = false)
	{
		mCurrentOffset = 0;
		Desc.Type = Type;
		Desc.NumDescriptors = NumDescriptors;
		Desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		Desc.NodeMask = 0;
		mHandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&mDescriptorHeap)));

		return true;
	}

	UINT& getCurrentOffsetRef() 
	{
		return mCurrentOffset;
	}

	void incrementCurrentOffset() 
	{
		mCurrentOffset++;
	}

	ID3D12DescriptorHeap* mPtr() const
	{
		return mDescriptorHeap.Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle(UINT index) 
	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(index, mHandleIncrementSize);
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle(UINT index) 
	{
		assert(Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		handle.Offset(index, mHandleIncrementSize);
		return handle;
	}
private:

	UINT mHandleIncrementSize;

	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHeapStart;
	UINT mCurrentOffset;
};