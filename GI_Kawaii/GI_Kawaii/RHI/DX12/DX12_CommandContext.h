#pragma once

class DX12_CommandContext
{
public:
	DX12_CommandContext();

	DX12_CommandContext(ID3D12Device* _device);

	DX12_CommandContext(const DX12_CommandContext& rhs) = delete;
	DX12_CommandContext& operator=(const DX12_CommandContext& rhs) = delete;

	void FlushCommandQueue();
	void EndCommandRecording();
	void ResetCommandList();
	void InitCommandContext();

	// Getters
	ID3D12CommandQueue* getQueuePtr();
	ID3D12CommandQueue** getQueuePPtr();

	ID3D12CommandAllocator* getAllocatorPtr();
	ID3D12CommandAllocator** getAllocatorPPtr();

	ID3D12GraphicsCommandList* getCommandListPtr();
	ID3D12GraphicsCommandList** getCommandListPPtr();

protected:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cpyQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cpyAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cpyCmdList;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 fenceValue;
	ID3D12Device* md3dDevice;
};