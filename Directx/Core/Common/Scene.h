#pragma once

class Scene
{
public:
	Scene();
	Scene(ID3D12Device* _device, UINT _mClientWidth, UINT _mClientHeight);

	Scene(const Scene& rhs) = delete;
	Scene& operator=(const Scene& rhs) = delete;

	void initScene();
	void resize(const int& width, const int& height);
private:
	UINT mClientWidth, mClientHeight;
	ID3D12Device* md3dDevice;
};