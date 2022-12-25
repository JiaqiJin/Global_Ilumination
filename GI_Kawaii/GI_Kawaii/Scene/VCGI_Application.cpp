#include "../pch.h"
#include "../Application.h"
#include "../Common/GeometryGenerator.h"
#include "../RHI/DX12/DX12_DescriptorHeap.h"

Application::Application(HINSTANCE hInstance)
	: Core(hInstance)
{
	// Estimate the scene bounding sphere manually since we know how the scene was constructed.
	// The grid is the "widest object" with a width of 20 and depth of 30.0f, and centered at
	// the world space origin.  In general, you need to loop over every world space vertex
	// position and compute the bounding sphere.
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);
}

void Application::OnDestroy()
{

}


Application::~Application()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void Application::CreateRtvAndDsvDescriptorHeaps()
{
	/*mRtvHeap = std::make_unique<DX12_DescriptorHeap>();
	mRtvHeap->CreateDescriptorHeap(md3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, (UINT)(SwapChainBufferCount), false);

	mDsvHeap = std::make_unique<DX12_DescriptorHeap>();
	mDsvHeap->CreateDescriptorHeap(md3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, (UINT)(2), false);*/

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = mRtvDescriptorSize;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	// 0 ==== dsv for default swapchain
	// 1 ==== dsv for shadow map

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = mDsvDescriptorSize;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

bool Application::Initialize()
{
	if (!Core::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Scene
	mScene = std::make_unique<Scene>(md3dDevice.Get(), mCommandList.Get(), mClientWidth, mClientHeight);
	mScene->InitScene();

	// Shadow map
	mShadowMap = std::make_unique<ShadowMap>(md3dDevice.Get(), 2048, 2048);

	// Deferred renderer
	mDefferedRenderer = std::make_unique<DeferredRenderer>(md3dDevice.Get(), mClientWidth, mClientHeight);
	mDefferedRenderer->InitDeferredRenderer();

	// Mesh Voxel
	mMeshVoxelizer = std::make_unique<MeshVoxelizer>(md3dDevice.Get(), 256, 256, 256);
	mMeshVoxelizer->Init3DVoxelTexture();

	CreateRtvAndDsvDescriptorHeaps();
	BuildDescriptorHeaps();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildFrameResources();
	BuildPSOs();
}

void Application::OnResize()
{
	Core::OnResize();
	XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtils::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);

	if (mScene.get()) {
		mScene->Resize(mClientWidth, mClientHeight);
	}
}

void Application::Update(const Timer& gt)
{
	OnKeyboardInput(gt);

	if (mShadowMap.get())
	{
		mShadowMap->Update(gt);
	}

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateCBs(gt);
}

void Application::Draw(const Timer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.

	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	// only need to set SRV CBV UAV here, since RTV and DSV will never be accessed inside a shader (only written to)
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvHeaps["MainPass"]->GetDescriptorHeap() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignatures["MainPass"].Get());
	mCommandList->SetGraphicsRootDescriptorTable(d3dUtil::MAIN_PASS_UNIFORM::SHADOWMAP_TEX_TABLE, mShadowMap->Srv());
	mCommandList->SetGraphicsRootDescriptorTable(d3dUtil::MAIN_PASS_UNIFORM::VOXEL, mMeshVoxelizer->getGPUHandle4SRV());

	DrawSceneToShadowMap();
	VoxelizeMesh();

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage. 
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, nullptr);
	
	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(d3dUtil::MAIN_PASS_UNIFORM::MAINPASS_CBV, passCB->GetGPUVirtualAddress());
	mCommandList->SetPipelineState(mPSOs["deferredPost"].Get());
	DrawRenderItems(mCommandList.Get(), mScene->getObjectInfoLayer()[(int)RenderLayer::Default]);
	mCommandList->SetPipelineState(mPSOs["debug"].Get());
	DrawRenderItems(mCommandList.Get(), mScene->getObjectInfoLayer()[(int)RenderLayer::Debug]);
	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void Application::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Application::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Application::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mScene->getCamerasMap()["MainCam"]->Pitch(dy);
		mScene->getCamerasMap()["MainCam"]->RotateY(dx);

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

void Application::OnKeyboardInput(const Timer& gt)
{
	const float dt = gt.DeltaTime();
	float speed = 30.0f;

	if (GetAsyncKeyState('1') & 0x8000)
		mIsWireframe = true;
	else
		mIsWireframe = false;
	if (GetAsyncKeyState('W') & 0x8000)
		mScene->getCamerasMap()["MainCam"]->Walk(speed * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mScene->getCamerasMap()["MainCam"]->Walk(-speed * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mScene->getCamerasMap()["MainCam"]->Strafe(-speed * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mScene->getCamerasMap()["MainCam"]->Strafe(speed * dt);
	mScene->getCamerasMap()["MainCam"]->UpdateViewMatrix();
}

void Application::UpdateCBs(const Timer& gt)
{
	UpdateObjectCBs(gt);
	UpdateMaterialCB(gt);
	UpdateMainPassCB(gt);
	UpdateShadowPassCB(gt);
}

void Application::UpdateObjectCBs(const Timer& gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mScene->getObjectInfos())
	{
		// Only update the cbuffer data if the constants have changed.  
	    // This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = DirectX::XMLoadFloat4x4(&e->texTransform);

			ObjectConstants objConstants;
			DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&objConstants.TexTransform, DirectX::XMMatrixTranspose(texTransform));
			objConstants.Obj2VoxelScale = e->Obj2VoxelScale;
			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void Application::UpdateMaterialCB(const Timer& gt)
{
	auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
	for (auto& e : mScene->getMaterialMap()) 
	{
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0) 
		{
			XMMATRIX matTransform = DirectX::XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			DirectX::XMStoreFloat4x4(&matConstants.MatTransform, DirectX::XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			mat->NumFramesDirty--;
		}
	}
}

void Application::UpdateCamera(const Timer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = DirectX::XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = DirectX::XMVectorZero();
	XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, view);
}

void Application::UpdateMainPassCB(const Timer& gt)
{
	PassConstants mMainPassCB;

	XMMATRIX view = mScene->getCamerasMap()["MainCam"]->GetView();
	XMMATRIX proj = mScene->getCamerasMap()["MainCam"]->GetProj();

	XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	XMMATRIX invView = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view), view);
	XMMATRIX invProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProj), viewProj);
	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowMap->mShadowMapData.mShadowTransform);
	XMMATRIX voxelView = XMLoadFloat4x4(&mMeshVoxelizer->getUniformData().mVoxelView);
	XMMATRIX voxelProj = XMLoadFloat4x4(&mMeshVoxelizer->getUniformData().mVoxelProj);
	XMMATRIX voxelViewProj = XMMatrixMultiply(voxelView, voxelProj);

	// use transpose to make sure we go from row - major on cpu to colume major on gpu
	DirectX::XMStoreFloat4x4(&mMainPassCB.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&mMainPassCB.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.ShadowTransform, DirectX::XMMatrixTranspose(shadowTransform));
	DirectX::XMStoreFloat4x4(&mMainPassCB.VoxelView, DirectX::XMMatrixTranspose(voxelView));
	DirectX::XMStoreFloat4x4(&mMainPassCB.VoxelProj, DirectX::XMMatrixTranspose(voxelProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.VoxelViewProj, DirectX::XMMatrixTranspose(voxelViewProj));
	mMainPassCB.EyePosW = mScene->getCamerasMap()["MainCam"]->GetPosition3f();
	mMainPassCB.LightPosW = mShadowMap->mShadowMapData.mLightPosW;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
}

void Application::UpdateShadowPassCB(const Timer& gt)
{
	PassConstants mShadowPassCB; 
	XMMATRIX view = DirectX::XMLoadFloat4x4(&mShadowMap->mShadowMapData.mLightView);
	XMMATRIX proj = DirectX::XMLoadFloat4x4(&mShadowMap->mShadowMapData.mLightProj);

	XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	XMMATRIX invView = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view), view);
	XMMATRIX invProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProj), viewProj);

	UINT w = mShadowMap->Width();
	UINT h = mShadowMap->Height();
	DirectX::XMStoreFloat4x4(&mShadowPassCB.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&mShadowPassCB.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&mShadowPassCB.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&mShadowPassCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&mShadowPassCB.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4(&mShadowPassCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	mShadowPassCB.EyePosW = mShadowMap->mShadowMapData.mLightPosW;
	mShadowPassCB.LightPosW = mShadowMap->mShadowMapData.mLightPosW;
	mShadowPassCB.RenderTargetSize = XMFLOAT2((float)w, (float)h);
	mShadowPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / w, 1.0f / h);
	mShadowPassCB.NearZ = mShadowMap->mShadowMapData.mLightNearZ;
	mShadowPassCB.FarZ = mShadowMap->mShadowMapData.mLightFarZ;
	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(1, mShadowPassCB);
}

void Application::BuildRootSignature()
{
	
}

void Application::BuildDescriptorHeaps()
{
	auto mMainPassSrvHeap = std::make_unique<DX12_DescriptorHeap>();
	mMainPassSrvHeap->CreateDescriptorHeap(md3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		(UINT)(mScene->getTexturesMap().size() + 1 + static_cast<UINT>(GBUFFER_TYPE::COUNT) + 2),
		true); // + 1 for shadow map + gbuffer + 2 for mesh voxelizer
	mSrvHeaps["MainPass"] = std::move(mMainPassSrvHeap);

	UINT offset = 0;
	for (auto& tex : mScene->getTexturesMap()) {
		auto texture = tex.second->mTextureBuffer;
		auto textureDesc = tex.second->getSRVDESC();
		md3dDevice->CreateShaderResourceView(texture.Get(), &textureDesc, mSrvHeaps["MainPass"]->GetCPUHandle(tex.second->textureID));
		mSrvHeaps["MainPass"]->getCurrentOffsetRef()++;
	}

	// set descriptor heap addresses for shadowmap

	auto shadowMapCPUSrvHandle = mSrvHeaps["MainPass"]->GetCPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	auto shadowMapGPUSrvHandle = mSrvHeaps["MainPass"]->GetGPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	mSrvHeaps["MainPass"]->getCurrentOffsetRef()++;
	auto dsvCPUstart = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	UINT dsvOffset = 1;
	auto shadowMapCPUDsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCPUstart, dsvOffset, mDsvDescriptorSize);
	mShadowMap->BuildDescriptors(shadowMapCPUSrvHandle, shadowMapGPUSrvHandle, shadowMapCPUDsvHandle);

	// set descriptor heap addresses for deferredrenderer

	auto rtvCPUstart = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	UINT rtvOffset = SwapChainBufferCount;

	for (auto& gbuffer : mDefferedRenderer->getGbuffersMap()) {
		auto deferredRendererCPURtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCPUstart, rtvOffset, mRtvDescriptorSize);
		auto deferredRendererCPUSrvHandle = mSrvHeaps["MainPass"]->GetCPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
		auto deferredRendererGPUSrvHandle = mSrvHeaps["MainPass"]->GetGPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
		gbuffer.second->SetupCPUGPUDescOffsets(deferredRendererCPUSrvHandle, deferredRendererGPUSrvHandle, deferredRendererCPURtvHandle);
		mSrvHeaps["MainPass"]->getCurrentOffsetRef()++;
		rtvOffset++;
	}

	// set descriptor heap addresses for deferredrenderer

	auto meshVoxelizerCPUSrvHandle = mSrvHeaps["MainPass"]->GetCPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	auto meshVoxelizerGPUSrvHandle = mSrvHeaps["MainPass"]->GetGPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	mSrvHeaps["MainPass"]->getCurrentOffsetRef()++;
	auto meshVoxelizerCPUUavHandle = mSrvHeaps["MainPass"]->GetCPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	auto meshVoxelizerGPUUavHandle = mSrvHeaps["MainPass"]->GetGPUHandle(mSrvHeaps["MainPass"]->getCurrentOffsetRef());
	mMeshVoxelizer->SetupCPUGPUDescOffsets(meshVoxelizerCPUSrvHandle, meshVoxelizerGPUSrvHandle, meshVoxelizerCPUUavHandle, meshVoxelizerGPUUavHandle);
}

void Application::BuildShadersAndInputLayout()
{
	
}

void Application::BuildPSOs()
{

}

void Application::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			2, (UINT)mAllRitems.size(), (UINT)mMaterials.size()));
	}
}

void Application::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<ObjectInfo*>& objInfos)
{

}

void Application::DrawSceneToShadowMap()
{
	
}

void Application::VoxelizeMesh()
{

}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> Application::GetStaticSamplers()
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
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,
		shadow
	};
}