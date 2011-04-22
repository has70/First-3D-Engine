#ifndef WINDOW_H
#define WINDOW_H


#include "Utilities.h"
#include "GameTimer.h"
#include <string>
using namespace std;


class Window
{
public:
	Window(HINSTANCE hInstance);
	virtual ~Window();

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	void resetOMTargetsAndViewport();

	int run();
	virtual void initApp();
	virtual void onResize();
	virtual void update(float dt);
	virtual void draw(); 
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	void initMainWindow();
	void initDirect3D();
	
protected:

	HINSTANCE hAppInst;
	HWND      hMainWnd;
	bool      paused;
	bool      minimized;
	bool      maximized;
	bool      resizing;

	GameTimer timer;
 
	ID3D10Device*    d3dDevice;
	IDXGISwapChain*  swapChain;
	ID3D10Texture2D* depthStencilBuffer;
	ID3D10RenderTargetView* renderTargetView;
	ID3D10DepthStencilView* depthStencilView;

	wstring mainWndCaption;
	D3D10_DRIVER_TYPE d3dDriverType;
	D3DXCOLOR clearColor;
	int windowWidth;
	int windowHeight;
};

#endif