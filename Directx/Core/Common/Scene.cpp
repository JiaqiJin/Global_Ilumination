#include "../pch.h"
#include "Scene.h"

Scene::Scene()
{

}

Scene::Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight)
	: md3dDevice(_device), mClientWidth(_mClientWidth), mClientHeight(_mClientHeight)
{

}

void Scene::initScene()
{

}

void Scene::resize(const int& width, const int& height)
{

}