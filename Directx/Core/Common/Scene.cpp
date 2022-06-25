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
    UINT curObjectIndex = 0;
    for (auto& curModel : mModels)
    {
        auto curObjectInfo = std::make_unique<ObjectInfo>();

        // populate information for each mesh
        for (auto& curMesh : curModel.second->getDrawArgs())
        {
            auto curDrawArg = curMesh.second;

            auto curMeshInfo = std::make_unique<MeshInfo>();
            curMeshInfo->Mat = curDrawArg.materialName;
            curMeshInfo->Model = curModel.second.get();
            curMeshInfo->IndexCount = curDrawArg.IndexCount;
            curMeshInfo->StartIndexLocation = curDrawArg.StartIndexLocation;
            curMeshInfo->BaseVertexLocation = curDrawArg.BaseVertexLocation;
            DirectX::XMStoreFloat4x4(&curMeshInfo->World, DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
            curObjectInfo->mMeshInfos.push_back(std::move(curMeshInfo));
        }

        // populate information for each model/obj
        curObjectInfo->Model = curModel.second.get();
        curObjectInfo->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        curObjectInfo->ObjCBIndex = curObjectIndex;
        curObjectInfo->World = curModel.second->getWorldMatrix();
        curObjectInfo->mBound = curModel.second->getBounds();
        curObjectInfo->Obj2VoxelScale = curModel.second->getObj2VoxelScale();
        curObjectInfo->IsDynamic = curModel.second->IsDynamic;

        //normal groups for generic rendering passes
        if (curModel.second->modelType == ModelType::QUAD_MODEL)
        {
            mObjectInfoLayer[(int)RenderLayer::Debug].push_back(curObjectInfo.get());
            mObjectInfoLayer[(int)RenderLayer::Gbuffer].push_back(curObjectInfo.get());
        }
        else 
        {
            mObjectInfoLayer[(int)RenderLayer::Default].push_back(curObjectInfo.get());
        }

        //dynamic and static geometry groups for revoxlizations
        if (curObjectInfo->IsDynamic) 
        {
            mObjectInfoLayer[(int)RenderLayer::Dynamic].push_back(curObjectInfo.get());
        }
        else
        {
            mObjectInfoLayer[(int)RenderLayer::Static].push_back(curObjectInfo.get());
        }

        mObjectInfos[curModel.first] = (std::move(curObjectInfo));

        curObjectIndex++;
    }
}

void Scene::buildMaterials() 
{
    // setup some customized materials here
    auto mat1 = std::make_unique<Material>();
    mat1->Name = "mat1";
    mat1->MatCBIndex = globalMatCBindex++;
    mat1->DiffuseAlbedo = DirectX::XMFLOAT4(0.2f, 0.6f, 0.6f, 1.0f);
    mat1->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
    mat1->Roughness = 1.0f;
    mat1->IsEmissive = false;
    mat1->DiffuseSrvHeapIndex = 0;
    mat1->NormalSrvHeapIndex = 0;

    auto mat2 = std::make_unique<Material>();
    mat2->Name = "mat2";
    mat2->MatCBIndex = globalMatCBindex++;
    mat2->DiffuseAlbedo = DirectX::XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
    mat2->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
    mat2->Roughness = 0.03;
    mat2->IsEmissive = false;
    mat2->DiffuseSrvHeapIndex = 0;
    mat2->NormalSrvHeapIndex = 0;

    auto mat3 = std::make_unique<Material>();
    mat3->Name = "mat3";
    mat3->MatCBIndex = globalMatCBindex++;
    mat3->DiffuseAlbedo = DirectX::XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
    mat3->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
    mat3->Roughness = 1.0;
    mat3->IsEmissive = false;
    mat3->DiffuseSrvHeapIndex = 2;
    mat3->NormalSrvHeapIndex = 0;

    mMaterials["mat1"] = std::move(mat1);
    mMaterials["mat2"] = std::move(mat2);
    mMaterials["mat3"] = std::move(mat3);
}

void Scene::loadTextures() 
{
    auto texture_1 = std::make_unique<Texture>("Path", globalTextureSRVDescriptorHeapIndex++);
    texture_1->Name = "tex1";
    texture_1->initializeTextureBuffer(md3dDevice, cpyCommandContext.get());

    mTextures[texture_1->Name] = std::move(texture_1);
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
    auto assimpModel = std::make_unique<Model>(ModelType::ASSIMP_MODEL);

    DirectX::XMFLOAT4X4 worldMat = MathUtils::Identity4x4();// 
    DirectX::XMStoreFloat4x4(&worldMat, DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
    assimpModel->setWorldMatrix(worldMat);
    assimpModel->setObj2VoxelScale(200.0f);

    Assimp::Importer importer;
    // importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
    const aiScene* aiscene = importer.ReadFile(filepath,

        aiProcess_Triangulate
        | aiProcess_GenNormals
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace
        | aiProcess_SortByPType
        | aiProcess_PreTransformVertices
    );

    if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode) {

        MessageBox(NULL, (L"ERROR::ASSIMP::" + AnsiToWString(importer.GetErrorString())).c_str(), L"Error!", MB_OK);
        return;
    }
    processNode(aiscene->mRootNode, aiscene, assimpModel.get());

    assimpModel->Name = filepath.substr(filepath.find_last_of('/') + 1);
    assimpModel->InitModel(md3dDevice, cpyCommandContext.get());
    mModels[assimpModel->Name] = std::move(assimpModel); 
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