#include "pch.h"
#include "WindowApplication.h"
#include <DirectXColors.h>
#include "Renderer/DeferredRenderer.h"

WindowApplication::WindowApplication(HINSTANCE hInstance)
	: Application(hInstance)
{

}

WindowApplication::~WindowApplication()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool WindowApplication::Initialize()
{
	if (!Application::Initialize())
		return false;

	mDeferredRenderer = std::make_unique<DeferredRenderer>(md3dDevice.Get(), mClientWidth, mClientHeight);
	mDeferredRenderer->Initialize();

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	BuildDescriptorHeaps();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void WindowApplication::OnResize()
{

}

void WindowApplication::Update(const GameTimer& gt)
{
	
}

void WindowApplication::OnKeyDown(WPARAM btnState)
{
	if (btnState == VK_TAB)
	{
		isFreeCamEnabled = !isFreeCamEnabled;
	}
}
void WindowApplication::OnKeyUp(WPARAM btnState)
{

}

void WindowApplication::OnMouseDown(WPARAM btnState, int x, int y) {
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void WindowApplication::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void WindowApplication::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (isFreeCamEnabled)
	{
		if ((btnState & MK_LBUTTON) != 0)
		{
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

			// Update angles based on input to orbit camera around box.
			mTheta += dx;
			mPhi += dy;

			// Restrict the angle mPhi.
			mPhi = MathUtils::Clamp(mPhi, 0.1f, MathUtils::Pi - 0.1f);
		}
		else if ((btnState & MK_RBUTTON) != 0)
		{
			// Make each pixel correspond to 0.2 unit in the scene.
			float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
			float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

			// Update the camera radius based on input.
			mRadius += dx - dy;

			// Restrict the radius.
			mRadius = MathUtils::Clamp(mRadius, 5.0f, 550.0f);
		}

		mLastMousePos.x = x;
		mLastMousePos.y = y;
	}
}

void WindowApplication::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	float speed = 60.0f;
}

void WindowApplication::Draw(const GameTimer& gt)
{
	
}

void WindowApplication::OnDestroy()
{

}

void WindowApplication::BuildDescriptorHeaps()
{
	
}

void WindowApplication::BuildRootSignature()
{
	
}

void WindowApplication::BuildShadersAndInputLayout()
{
	
}

void WindowApplication::BuildPSO()
{
	
}

void WindowApplication::CreatePSO(ID3D12PipelineState** pso,
	ID3D12RootSignature* rootSignature,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopoType,
	D3D12_BLEND_DESC blendDesc,
	D3D12_RASTERIZER_DESC rasterDesc,
	D3D12_DEPTH_STENCIL_DESC dsState,
	UINT numRenderTargets,
	DXGI_FORMAT renderTargetFormat,
	DXGI_FORMAT depthStencilFormat,
	ID3DBlob* vertexShader,
	ID3DBlob* pixelShader,
	ID3DBlob* geometryShader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	PsoDesc.pRootSignature = rootSignature;
	if (vertexShader) 
	{
		PsoDesc.VS = d3dUtil::getShaderBytecode(vertexShader);
	}
	if (pixelShader) 
	{
		PsoDesc.PS = d3dUtil::getShaderBytecode(pixelShader);
	}
	if (geometryShader)
	{
		PsoDesc.GS = d3dUtil::getShaderBytecode(geometryShader);
	}
	PsoDesc.RasterizerState = rasterDesc;
	PsoDesc.BlendState = blendDesc;
	PsoDesc.DepthStencilState = dsState;
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = primitiveTopoType;
	PsoDesc.NumRenderTargets = numRenderTargets;

	if (numRenderTargets == 0)
		PsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	for (unsigned int i = 0; i < numRenderTargets; ++i) 
	{
		PsoDesc.RTVFormats[i] = renderTargetFormat;
	}
	PsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	PsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	PsoDesc.DSVFormat = depthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(pso)));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> WindowApplication::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		0,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0.f,
		D3D12_FLOAT32_MAX);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}