#pragma once

enum class GBUFFER_TYPE : int
{ 
	POSITION = 0,
	ALBEDO, 
	NORMAL,
	DEPTH, 
	COUNT 
};

class GBuffer
{
public:
	GBuffer(ID3D12Device* device, UINT width, UINT heigth);

	GBuffer(const GBuffer& rhs) = delete;
	GBuffer& operator=(const GBuffer& rhs) = delete;
	~GBuffer() = default;

	bool Initialize();
	void OnResize(UINT newWidth, UINT newHeight);

	void SetupCPUGPUDescOffsets(
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuRtv);

	// Getters
	ID3D12Resource* GetResourcePtr() { return mTexture.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleSRV() const { return mCPUSRV; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleSRV() const { return mGPUSRV; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleRTV() const { return mCPURTV; }

private:
	void BuildDescriptors();
	void BuildResources();

private:
	ID3D12Device* device;
	UINT mWidth = 0;
	UINT mHeight = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUSRV;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPURTV;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUSRV;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Microsoft::WRL::ComPtr<ID3D12Resource> mTexture;
};

class DeferredRenderer
{
public:
	DeferredRenderer();
	DeferredRenderer(ID3D12Device* device, UINT width, UINT height);

	void Initialize();
	void onResize(UINT newWidth, UINT newHeight);
private:
	ID3D12Device* md3dDevice = nullptr;

	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;

	std::vector<std::unique_ptr<GBuffer>> mGBuffers;
};