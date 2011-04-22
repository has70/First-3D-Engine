#include "Window.h"
#include "DrawManager.h"

class Engine : public Window
{
private:
	DrawManager drawManager;
public:
	Engine(HINSTANCE hInstance);
	~Engine();

	void initApp();
	void onResize();
	void update(float dt);
	void draw(); 
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//Detect memory leaks
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//Init engine
	Engine engine(hInstance);
	engine.initApp();

	//Run engine
	return engine.run();
}

Engine::Engine(HINSTANCE hInstance): Window(hInstance) 
{
}

Engine::~Engine()
{
	if(d3dDevice)
		d3dDevice->ClearState();
}

void Engine::initApp()
{
	Window::initApp();
	drawManager.init(d3dDevice);
}

void Engine::onResize()
{
	Window::onResize();

	float aspect = (float)windowWidth/windowHeight;
	drawManager.onResize(aspect);
}

void Engine::update(float dt)
{
	//Update game
	drawManager.update(dt);
}

void Engine::draw()
{
	//Clear render target and depth buffer
	Window::draw();

	//Draw game
	drawManager.draw();

	swapChain->Present(0, 0);
}
