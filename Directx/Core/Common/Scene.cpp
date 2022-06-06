#include "../pch.h"
#include "Scene.h"
#include "../DX12/CommandContext.h"

Scene::Scene()
{
	mObjectInfoLayer.resize((int)RenderLayer::Count);
}

Scene::Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight)
	: md3dDevice(_device), mClientWidth(_mClientWidth), mClientHeight(_mClientHeight) 
{
	mObjectInfoLayer.resize((int)RenderLayer::Count);
	cpyCommandContext = std::make_shared<CommandContext>(md3dDevice);
}

std::unordered_map<std::string, std::unique_ptr<Material>>& Scene::getMaterialMap() 
{
	return mMaterials;
}

std::unordered_map<std::string, std::unique_ptr<Model>>& Scene::getModelsMap()
{
	return mModels;
}

std::unordered_map<std::string, std::unique_ptr<Camera>>& Scene::getCamerasMap()
{
	return mCameras;
}

std::unordered_map<std::string, std::unique_ptr<Texture>>& Scene::getTexturesMap()
{
    return mTextures;
}

std::unordered_map<std::string, std::unique_ptr<ObjectInfo>>& Scene::getObjectInfos()
{
	return mObjectInfos;
}

const std::vector<std::vector<ObjectInfo*>>& Scene::getObjectInfoLayer() 
{
	return mObjectInfoLayer;
}

void Scene::populateMeshInfos()
{

}

void Scene::buildMaterials() 
{
   
}

void Scene::loadTextures() 
{
   
}

void Scene::buildCameras() {

    auto normalCam = std::make_unique<Camera>();
    normalCam->SetPosition(0.0f, 192.0f, -115.0f);
    normalCam->SetLens(0.25f * MathUtils::Pi, static_cast<float>(mClientWidth) / mClientHeight, 1.0f, 1000.0f);
    normalCam->LookAt(DirectX::XMFLOAT3(220.0, 242.0, 95.0), DirectX::XMFLOAT3(0.0, 0.0, 0.0), DirectX::XMFLOAT3(0.0, 1.0, 0.0));
    mCameras["MainCam"] = std::move(normalCam);
}

void Scene::loadModels()
{
    auto myquad = std::make_unique<Model>(ModelType::QUAD_MODEL, "mat1");
    myquad->Name = "quad";
    myquad->InitModel(md3dDevice, cpyCommandContext.get());
    mModels["quad"] = std::move(myquad);
}

void Scene::loadAssetFromAssimp(const std::string filepath)
{

}

void Scene::processNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel) {

    for (unsigned int i = 0; i < ainode->mNumMeshes; ++i) {

        aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[i]];

        assimpModel->appendAssimpMesh(aiscene, aimesh);

    }
    for (unsigned int i = 0; i < ainode->mNumChildren; ++i) {
        processNode(ainode->mChildren[i], aiscene, assimpModel);
    }
}

void Scene::initScene()
{
    cpyCommandContext->FlushCommandQueue();
    cpyCommandContext->resetCommandList();

    buildCameras();
    loadModels();
    buildMaterials();
    loadTextures();
    loadAssetFromAssimp("");

    cpyCommandContext->endCommandRecording();
    cpyCommandContext->FlushCommandQueue();

    populateMeshInfos();
}

void Scene::resize(const int& _w, const int& _h)
{
    mClientWidth = _w;
    mClientHeight = _h;
    mCameras["MainCam"]->SetLens(0.25f * MathUtils::Pi, static_cast<float>(mClientWidth) / mClientHeight, 1.0f, 1000.0f);
}