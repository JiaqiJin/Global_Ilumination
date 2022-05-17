#include "pch.h"
#include "WindowApplication.h"
#include <DirectXColors.h>

WindowApplication::WindowApplication(HINSTANCE hInstance)
	: Application(hInstance)
{

}

WindowApplication::~WindowApplication()
{

}

bool WindowApplication::Initialize()
{
	if (!Application::Initialize())
		return false;

	return true;
}

void WindowApplication::Update(const GameTimer& gt)
{

}

void WindowApplication::Draw(const GameTimer& gt)
{

}

void WindowApplication::OnDestroy()
{

}