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
	if( d3dDevice )
		d3dDevice->ClearState();
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
	font_tf2_medium->DrawText(0, text_fps.c_str(), -1, &R, DT_NOCLIP, WHITE);

	swapChain->Present(0, 0);
}
