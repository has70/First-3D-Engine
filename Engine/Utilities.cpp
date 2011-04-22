#include "Utilities.h"

////Build FX
//void buildEffect(string fileName, ID3D10Device* device, ID3D10Effect* effect)
//{
//	//Debug flag
//	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//	shaderFlags |= D3D10_SHADER_DEBUG;
//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#endif
//	ID3D10Blob* compilationErrors = 0;
//	HRESULT hr = 0;
//
//	hr = D3DX10CreateEffectFromFile(string_to_wstring(fileName).c_str(), 0, 0, "fx_4_0", shaderFlags, 0, device, 0, 0, &effect, &compilationErrors, 0);
//	if(FAILED(hr))
//	{
//		if(compilationErrors)
//		{
//			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
//			ReleaseCOM(compilationErrors);
//		}
//		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
//	} 
//}


//String to Wstring
wstring string_to_wstring(string in)
{
	wstring out(in.length(), L' ');
	copy(in.begin(), in.end(), out.begin());
	return out;
}