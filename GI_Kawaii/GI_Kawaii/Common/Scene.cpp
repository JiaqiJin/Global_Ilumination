#include "../pch.h"
#include "Scene.h"

Scene::Scene()
{

}

Scene::Scene(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList, UINT _mClientWidth, UINT _mClientHeight)
	: md3dDevice(_device), cmdList(_cmdList), mClientWidth(_mClientWidth), mClientHeight(_mClientHeight)
{

}

void Scene::InitScene()
{
	LoadModels();
}

void Scene::BuildMaterials()
{

}

void Scene::LoadTextures()
{

}

void Scene::PopulateMeshInfos()
{

}

void Scene::BuildCameras()
{

}

void Scene::LoadModels()
{
	auto TestModel = std::make_unique<Model>(ModelType::TEMPLATE_MODEL);
	TestModel->Name = "TestModel";
	mModels[TestModel->Name] = std::move(TestModel);
    TestModel->InitModel(md3dDevice, cmdList);
    TestModel->setObj2VoxelScale(200.0f);
    mModels["model1"] = std::move(TestModel);
}

void Scene::LoadAssetFromAssimp(const std::string filepath)
{
    auto assimpModel = std::make_unique<Model>(ModelType::ASSIMP_MODEL);

    DirectX::XMFLOAT4X4 worldMat = MathUtils::Identity4x4();// 
    DirectX::XMStoreFloat4x4(&worldMat, DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
    assimpModel->SetWorldMatrix(worldMat);
    assimpModel->setObj2VoxelScale(200.0f);

    //==============================================
    // assimp init
    //===========================================

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

    //=============================================
    // mesh appending (CPU)
    //===========================================

    ProcessNode(aiscene->mRootNode, aiscene, assimpModel.get());

    assimpModel->Name = filepath.substr(filepath.find_last_of('/') + 1);
    assimpModel->InitModel(md3dDevice, cmdList);
    mModels[assimpModel->Name] = std::move(assimpModel);

    //=============================================
    // material construct
    //===========================================

    const std::string directory = filepath.substr(0, filepath.find_last_of('/')) + '/';    // get the directory path
    for (unsigned int i = 0; i < aiscene->mNumMaterials; ++i) {
        aiMaterial* curMaterial = aiscene->mMaterials[i];
        UINT srvDiffID = 0;
        UINT srvNormID = 0;
        for (unsigned int j = 0; j < curMaterial->GetTextureCount(aiTextureType_DIFFUSE); ++j)
        {

           
        }
        for (unsigned int j = 0; j < curMaterial->GetTextureCount(aiTextureType_NORMALS); ++j)
        {
           
        }
       
    }
}

void Scene::ProcessNode(aiNode* ainode, const aiScene* aiscene, Model* assimpModel)
{
	for (unsigned int i = 0; i < ainode->mNumMeshes; ++i) 
	{

		aiMesh* aimesh = aiscene->mMeshes[ainode->mMeshes[i]];

		assimpModel->AppendAssimpMesh(aiscene, aimesh);
	}

	for (unsigned int i = 0; i < ainode->mNumChildren; ++i)
	{
		ProcessNode(ainode->mChildren[i], aiscene, assimpModel);
	}
}

void Scene::Resize(const int width, const int height)
{
	mClientWidth = width;
	mClientHeight = height;
	mCameras["MainCam"]->SetLens(0.25f * MathUtils::Pi, static_cast<float>(mClientWidth) / mClientHeight, 1.0f, 1000.0f);
}

std::unordered_map<std::string, std::unique_ptr<Material>>& Scene::getMaterialMap()
{
	return mMaterials;
}

std::unordered_map<std::string, std::unique_ptr<Texture>>& Scene::getTexturesMap()
{
	return mTextures;
}

std::unordered_map<std::string, std::unique_ptr<Model>>& Scene::getModelsMap()
{
	return mModels;
}

std::unordered_map<std::string, std::unique_ptr<Camera>>& Scene::getCamerasMap()
{
	return mCameras;
}

std::unordered_map<std::string, std::unique_ptr<ObjectInfo>>& Scene::getObjectInfos()
{
	return mObjectInfos;
}