#include "../pch.h"
#include "DeferredRenderer.h"

GBuffer::GBuffer(ID3D12Device* _device, UINT width, UINT heigth)
	: device(_device), mTexture(nullptr), mWidth(width), mHeight(heigth)
{

}

void GBuffer::SetupCPUGPUDescOffsets(D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv, 
	D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv, D3D12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
	mCPUSRV = hCpuSrv;
	mCPURTV = hCpuRtv;
	hGpuSrv = hGpuSrv;
	BuildDescriptors();
}

bool GBuffer::Initialize()
{
	BuildResources();
	return true;
}

void GBuffer::OnResize(UINT newWidth, UINT newHeight)
{
	if ((mWidth != newWidth) || (mHeight != newHeight))
	{
		mHeight = newHeight;
		mWidth = newWidth;
		BuildResources();
		BuildDescriptors();
	}
}

void GBuffer::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	device->CreateShaderResourceView(mTexture.Get(), &srvDesc, mCPUSRV);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = mFormat;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	device->CreateRenderTargetView(mTexture.Get(), &rtvDesc, mCPURTV);
}

void GBuffer::BuildResources()
{
	// release memory here in case we are resize the gbuffers
	mTexture.Reset();

	D3D12_RESOURCE_DESC texDesc = {};
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CD3DX12_CLEAR_VALUE optClear(mFormat, clearColor);
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(mTexture.GetAddressOf())
	));
}

//------------------------ Deferred -------------------------
DeferredRenderer::DeferredRenderer()
{

}

DeferredRenderer::DeferredRenderer(ID3D12Device* device, UINT width, UINT height)
	: md3dDevice(device), mWidth(width), mHeight(height) 
{
	mViewPort = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mScissorRect = { 0,0,(int)width, (int)height };
}

void DeferredRenderer::Initialize()
{

}

void DeferredRenderer::onResize(UINT newWidth, UINT newHeight)
{
	if ((mWidth != newWidth) || (mHeight != newHeight)) 
	{
		mHeight = newHeight;
		mWidth = newWidth;
		mViewPort = { 0.0f, 0.0f, (float)newWidth, (float)newHeight, 0.0f, 1.0f };
		mScissorRect = { 0,0,(int)newWidth, (int)newHeight };
		
	}
}