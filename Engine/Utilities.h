#ifndef UTILS_H
#define UTILS_H

#include <d3dx10.h>
#include <dxerr.h>
#include <cassert>
#include <vector>
using namespace std;

//Error checker
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif

//Release COM objects
#define ReleaseCOM(x){ if(x){ x->Release();x = 0;}}

//Build FX
//void buildEffect(string fileName, ID3D10Device* device, ID3D10Effect* effect);
wstring string_to_wstring(string in);

//Constants
const float INFINITY = FLT_MAX;
const float PI       = 3.14159265358979323f;
const float MATH_EPS = 0.0001f;

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const D3DXCOLOR YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
//const D3DXCOLOR CYAN(0.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR CYAN(0.1f,0.7f,0.7f,1.0f);
const D3DXCOLOR MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

const D3DXCOLOR CRIMSON(0.8f, 0.1f, 0.1f, 1.0f);


#endif