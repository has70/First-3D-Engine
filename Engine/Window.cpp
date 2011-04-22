#include "Resource.h"
#include "Window.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Window* app = 0;

	switch(msg)
	{
		case WM_CREATE:
		{
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			app = (Window*)cs->lpCreateParams;
			return 0;
		}
	}

	// Don't start processing messages until after WM_CREATE.
	if(app)
		return app->msgProc(msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window(HINSTANCE hInstance)
{
	hAppInst   = hInstance;
	hMainWnd   = 0;
	paused	   = false;
	minimized  = false;
	maximized  = false;
	resizing   = false;
 
	d3dDevice          = 0;
	swapChain          = 0;
	depthStencilBuffer = 0;
	renderTargetView   = 0;
	depthStencilView   = 0;

	mainWndCaption = L"Vivarium";
	d3dDriverType  = D3D10_DRIVER_TYPE_HARDWARE;
	clearColor     = D3DXCOLOR(CRIMSON);
	windowWidth    = 800;
	windowHeight   = 600;
}

Window::~Window()
{
	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(swapChain);
	ReleaseCOM(depthStencilBuffer);
	ReleaseCOM(d3dDevice);
}

HINSTANCE Window::getAppInst()
{
	return hAppInst;
}

HWND Window::getMainWnd()
{
	return hMainWnd;
}

void Window::resetOMTargetsAndViewport()
{
	d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 100;
    vp.TopLeftY = 100;
    vp.Width    = 400;
    vp.Height   = 300;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

	d3dDevice->RSSetViewports(1, &vp);
}

int Window::run()
{
	MSG msg = {0};
	timer.reset();

	while(msg.message != WM_QUIT)
	{
		//If Window messages, process them
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		//Otherwise, do game stuff
		else
        {	
			timer.tick();
			if(!paused)
				update(timer.getDeltaTime());	
			else
				Sleep(50);
			draw();
        }
    }
	return (int)msg.wParam;
}

void Window::initApp()
{
	initMainWindow();
	initDirect3D();
}
 
void Window::onResize()
{
	//Release old views so we can create new buffers
	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(depthStencilBuffer);


	//Resize the swap chain and recreate the render target view
	HR(swapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D10Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseCOM(backBuffer);


	//Create depth/stencil buffer and view
	D3D10_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = windowWidth;
	depthStencilDesc.Height    = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; //Must match swap chain values.
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
	HR(d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(d3dDevice->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));


	//Bind render target view and depth/stencil view to pipeline
	d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	

	//Set viewport
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = windowWidth;
	vp.Height   = windowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	d3dDevice->RSSetViewports(1, &vp);
}

void Window::update(float dt)
{
}

void Window::draw()
{
	d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
	d3dDevice->ClearDepthStencilView(depthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

LRESULT Window::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	//Pause game if window is deactivated
	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
		{
			paused = true;
			timer.stop();
		}
		else
		{
			paused = false;
			timer.start();
		}
		return 0;

	//If window is resized
	case WM_SIZE:
		//Save new dimensions
		windowWidth  = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		if(d3dDevice)
		{
			if(wParam == SIZE_MINIMIZED)
			{
				paused = true;
				minimized = true;
				maximized = false;
			}
			else if(wParam == SIZE_MAXIMIZED)
			{
				paused = false;
				minimized = false;
				maximized = true;
				onResize();
			}
			else if(wParam == SIZE_RESTORED)
			{
				
				//Restoring from minimized state?
				if(minimized)
				{
					paused = false;
					minimized = false;
					onResize();
				}

				//Restoring from maximized state?
				else if(maximized)
				{
					paused = false;
					maximized = false;
					onResize();
				}
				else if(resizing)
				{
					//If user is dragging the resize bars, wait until user is done before resize 
				}
				else //If SetWindowPos or SetFullscreenState.
				{
					onResize();
				}
			}
		}
		return 0;

	//If user grabs the resize bars
	case WM_ENTERSIZEMOVE:
		paused = true;
		resizing  = true;
		timer.stop();
		return 0;

	//If user releases the resize bars
	case WM_EXITSIZEMOVE:
		paused = false;
		resizing  = false;
		timer.start();
		onResize();
		return 0;
 
	//If window is being destroyed
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key
	case WM_MENUCHAR:
        //Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);
 
	//Prevent window from becoming too small
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	}

	return DefWindowProc(hMainWnd, msg, wParam, lParam);
}


void Window::initMainWindow()
{
	//Create windowclass
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hAppInst;
	//wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	//Use custom icon
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));

	//Register windowclass
	if(!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}


	//Compute window rectangle
	RECT R = {0, 0, windowWidth, windowHeight};
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	//Create window
	hMainWnd = CreateWindow(L"D3DWndClassName", mainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hAppInst, this); 
	if(!hMainWnd)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(hMainWnd, SW_SHOW);
	UpdateWindow(hMainWnd);
}

void Window::initDirect3D()
{
	//Create swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = windowWidth;
	sd.BufferDesc.Height = windowHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = hMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;


	//Create DirectX device.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	HR(D3D10CreateDeviceAndSwapChain(0, d3dDriverType, 0, createDeviceFlags, D3D10_SDK_VERSION, &sd, &swapChain, &d3dDevice));


	//Create backbuffer etc, used here to prevents duplication of code
	onResize();
}


