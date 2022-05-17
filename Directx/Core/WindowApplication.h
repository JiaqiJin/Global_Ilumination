#pragma once

#include "Common/Application.h"
using namespace DirectX;

class WindowApplication : public Application
{
public:
	WindowApplication(HINSTANCE hInstance);
	~WindowApplication();

	virtual bool Initialize()override;
private:
	//virtual void OnResize()override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void OnDestroy() override;
};