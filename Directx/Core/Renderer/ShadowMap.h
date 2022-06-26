#pragma once

struct ShadowMapData
{

};

class ShadowMap
{
public:

	ShadowMap();
	ShadowMap(ID3D12Device* device, UINT width, UINT height);

	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;
	~ShadowMap() = default;

	UINT Width()const;
	UINT Height()const;
	D3D12_VIEWPORT Viewport()const;
	D3D12_RECT ScissorRect()const;
	ID3D12Resource* getResourcePtr();
	D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle4SRV()const;
	D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle4DSV()const;

	void SetupCPUGPUDescOffsets(
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	void OnResize(UINT newWidth, UINT newHeight);
	void Update(const GameTimer& gt);

	bool initShadowMap();

	ShadowMapData mShadowMapData;
	void setShadowLightPos(float x, float y, float z, const GameTimer& gt);

private:
	void BuildDescriptors();
	void BuildResource();

private:

	ID3D12Device* md3dDevice = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

	D3D12_CPU_DESCRIPTOR_HANDLE mhCPUSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE mhGPUSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE mhCPUDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
};

