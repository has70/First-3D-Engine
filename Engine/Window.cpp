#include "Window.h"
#include <sstream>

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

	text_fps = L"";
 
	d3dDevice          = 0;
	swapChain          = 0;
	depthStencilBuffer = 0;
	renderTargetView   = 0;
	depthStencilView   = 0;

	mainWndCaption = L"Modus Vivendi";
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
	ReleaseCOM(font_tf2_medium);
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
				updateScene(timer.getDeltaTime());	
			else
				Sleep(50);
			drawScene();
        }
    }
	return (int)msg.wParam;
}

void Window::initApp()
{
	initMainWindow();
	initDirect3D();

	//Add custom fonts
	HR(AddFontResource(L"Data/Fonts/tf2build.ttf"));

	//Init font
	D3DX10_FONT_DESC fontDesc;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy(fontDesc.FaceName, L"TF2 Build");

	//Create fonts
	fontDesc.Height = 18;
	D3DX10CreateFontIndirect(d3dDevice, &fontDesc, &font_tf2_small);
	fontDesc.Height = 24;
	D3DX10CreateFontIndirect(d3dDevice, &fontDesc, &font_tf2_medium);
	fontDesc.Height = 45;
	D3DX10CreateFontIndirect(d3dDevice, &fontDesc, &font_tf2_large);
}
 
void Window::onResize()
{
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(depthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(swapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D10Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseCOM(backBuffer);


	// Create the depth/stencil buffer and view.

	D3D10_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = windowWidth;
	depthStencilDesc.Height    = windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(d3dDevice->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	

	// Set the viewport transform.

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = windowWidth;
	vp.Height   = windowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	d3dDevice->RSSetViewports(1, &vp);
}

void Window::updateScene(float dt)
{
	static int nrOfFrames=0;
	static float time_base = 0.0f;
	nrOfFrames++;

	//Compute averages over one second period.
	if((timer.getGameTime()-time_base) >= 1.0f)
	{
		float fps = (float)nrOfFrames;
		float mspf = 1000.0f/fps; //Milliseconds per frame

		wostringstream s;
		s << L"FPS " << fps;
		s.precision(3);
		s << L"\n" << "MPF " << mspf;
		text_fps = s.str();
		
		//Reset for next average.
		nrOfFrames = 0;
		time_base  += 1.0f;
	}
}

void Window::drawScene()
{
	d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
	d3dDevice->ClearDepthStencilView(depthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

LRESULT Window::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
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

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		windowWidth  = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		if( d3dDevice )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				paused = true;
				minimized = true;
				maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				paused = false;
				minimized = false;
				maximized = true;
				onResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( minimized )
				{
					paused = false;
					minimized = false;
					onResize();
				}

				// Restoring from maximized state?
				else if( maximized )
				{
					paused = false;
					maximized = false;
					onResize();
				}
				else if( resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or swapChain->SetFullscreenState.
				{
					onResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		paused = true;
		resizing  = true;
		timer.stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		paused = false;
		resizing  = false;
		timer.start();
		onResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);
 
	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	}

	return DefWindowProc(hMainWnd, msg, wParam, lParam);
}


void Window::initMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	hMainWnd = CreateWindow(L"D3DWndClassName", mainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hAppInst, this); 
	if( !hMainWnd )
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(hMainWnd, SW_SHOW);
	UpdateWindow(hMainWnd);
}

void Window::initDirect3D()
{
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = windowWidth;
	sd.BufferDesc.Height = windowHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// No multisampling.
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = hMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;


	// Create the device.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	HR( D3D10CreateDeviceAndSwapChain(0,d3dDriverType,0,createDeviceFlags, D3D10_SDK_VERSION,&sd,&swapChain,&d3dDevice) );

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the onResize method here to avoid code duplication.
	
	onResize();
}


