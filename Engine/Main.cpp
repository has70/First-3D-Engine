#include "Window.h"

class Engine : public Window
{
public:
	Engine(HINSTANCE hInstance);
	~Engine();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Engine engine(hInstance);
	engine.initApp();

	return engine.run();
}

Engine::Engine(HINSTANCE hInstance): Window(hInstance) 
{
}

Engine::~Engine()
{
	if( md3dDevice )
		md3dDevice->ClearState();
}

void Engine::initApp()
{
	Window::initApp();
}

void Engine::onResize()
{
	Window::onResize();
}

void Engine::updateScene(float dt)
{
	Window::updateScene(dt);
}

void Engine::drawScene()
{
	Window::drawScene();

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}
