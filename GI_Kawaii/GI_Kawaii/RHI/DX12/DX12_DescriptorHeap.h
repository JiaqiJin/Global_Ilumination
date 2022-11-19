#pragma once

class DX12_DescriptorHeap
{
public:
	DX12_DescriptorHeap();

	bool CreateDescriptorHeap(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type,
		UINT NumDescriptors, bool bShaderVisible = false);

	UINT& getCurrentOffsetRef() { return mCurrentOffset; }
	void incrementCurrentOffset() { mCurrentOffset++; }
	ID3D12DescriptorHeap* GetDescriptorHeap() { return m_DescriptorHeap.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index);
private:
	UINT mHandleIncrementSize;
	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHeapStart;
	UINT mCurrentOffset;
};