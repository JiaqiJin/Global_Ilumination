#pragma once

class CommandContext
{
public:
	CommandContext();

	CommandContext(ID3D12Device* _device);

	CommandContext(const CommandContext& rhs) = delete;
	CommandContext& operator=(const CommandContext& rhs) = delete;

	void FlushCommandQueue();

	void endCommandRecording();

	void resetCommandList();

	void InitCommandContext();

	ID3D12CommandQueue* getQueuePtr();
	ID3D12CommandQueue** getQueuePPtr();

	ID3D12CommandAllocator* getAllocatorPtr();
	ID3D12CommandAllocator** getAllocatorPPtr();

	ID3D12GraphicsCommandList* getCommandListPtr();
	ID3D12GraphicsCommandList** getCommandListPPtr();

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cpyQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cpyAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cpyCmdList;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 fenceValue;
	ID3D12Device* md3dDevice;
};