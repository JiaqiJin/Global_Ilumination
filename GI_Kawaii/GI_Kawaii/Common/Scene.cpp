#include "../pch.h"
#include "Scene.h"

Scene::Scene()
{

}

Scene::Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight)
	: md3dDevice(_device), mClientWidth(_mClientWidth), mClientHeight(_mClientHeight)
{

}

void Scene::InitScene()
{
	LoadModels();
}

void Scene::LoadModels()
{
	auto TestModel = std::make_unique<Model>(ModelType::TEMPLATE_MODEL);
	TestModel->Name = "TestModel";

	// TODO
	mModels[TestModel->Name] = std::move(TestModel);
}

void Scene::LoadAssetFromAssimp(const std::string filepath)
{
	auto AssimpMode = new Model(ModelType::ASSIMP_MODEL);

	DirectX::XMFLOAT4X4 worldMat = MathUtils::Identity4x4();// the sponza model is stupidly big, I'm not happy bout dat
	DirectX::XMStoreFloat4x4(&worldMat, DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	AssimpMode->SetWorldMatrix(worldMat);

	//==============================================
   // assimp init
   //===========================================

	Assimp::Importer importer;
	// importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	const aiScene* aiscene = importer.ReadFile(filepath,

		aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_CalcTangentSpace
	);

	if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode) {

		MessageBox(NULL, (L"ERROR::ASSIMP::" + AnsiToWString(importer.GetErrorString())).c_str(), L"Error!", MB_OK);
		return;
	}

	//=============================================
	// mesh appending (CPU)
	//===========================================

	ProcessNode(aiscene->mRootNode, aiscene, AssimpMode);

	AssimpMode->Name = filepath.substr(filepath.find_last_of('/') + 1);
	//AssimpMode->InitModel(md3dDevice.Get(), mCommandList.Get());
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
	// TODO
}