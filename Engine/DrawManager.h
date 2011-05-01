#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include "Utilities.h"
#include "DrawFonts.h"
#include "Box.h"

class DrawManager
{
private:
	ID3D10Device* device;
	ID3D10Effect* effect;
	ID3D10EffectTechnique* technique;
	ID3D10InputLayout* layout;
	
	DrawFonts drawText;
	Box box;

	//Camera
	D3DXMATRIX mProj;
	D3DXMATRIX world;

public:
	DrawManager()
	{
		device=0;
		effect=0;
		technique=0;
		layout=0;
	};
	~DrawManager()
	{
		ReleaseCOM(effect);
		ReleaseCOM(layout);
	};
	void init(ID3D10Device* device)
	{
		this->device=device;
		drawText.init(device);
		createFX();
		buildVertexLayouts();
		box.init(device,1.0f);
	};

	void onResize(float aspect)
	{
		//Recalculate Field of View
		D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
	};

	void update(float dt)
	{
		//Rotate models
		static float rot=0.0f;
		rot+=dt/10;
		D3DXMatrixRotationY(&world, 2*PI*rot);

		//Update text
		drawText.update(dt);
	};

	void draw()
	{
		static ID3D10EffectMatrixVariable* wvp=effect->GetVariableByName("gWVP")->AsMatrix();
		D3DXMATRIX mWVP,mView;

		D3DXVECTOR3 pos(0, 0, -10);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&mView, &pos, &target, &up);

		mWVP = mView*mProj;


		device->OMSetDepthStencilState(0, 0);
		float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
		device->OMSetBlendState(0, blendFactors, 0xffffffff);
		device->IASetInputLayout(layout);
		device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		

		D3D10_TECHNIQUE_DESC techDesc;
		technique->GetDesc(&techDesc);
		for(unsigned i=0; i<techDesc.Passes; i++)
		{
			ID3D10EffectPass* pass = technique->GetPassByIndex(i);
			

			//for(int x=0; x<10000; x++)
			//{
			//	//Translate
			//	D3DXMATRIX mat_tmp;
			//	D3DXVECTOR3 v((float)x,0.0f,(float)x*2);
			//	if(x % 2)
			//		v=D3DXVECTOR3((float)-x,0.0f,(float)x*2);
			//	D3DXMatrixTranslation(&mat_tmp,v.x,v.y,v.z);

			//	//Rotate

			//	mWVP = world*mat_tmp*mView*mProj;
			//	wvp->SetMatrix((float*)&mWVP);

			//	pass->Apply(0);
			//	box.draw();
			//}
			mWVP = world*mView*mProj;
			wvp->SetMatrix((float*)&mWVP);
			pass->Apply(0);
			box.draw();
		}

		drawText.drawFPS();
	};
	UINT test;
	void createFX()
	{
		buildEffect("shader.fx");
		technique = effect->GetTechniqueByName("ColorTech");
	};

	void buildEffect(string fileName)
	{
		//Debug flag
		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		ID3D10Blob* compilationErrors = 0;
		HRESULT hr = 0;

		//Create effect from file
		hr = D3DX10CreateEffectFromFile(string_to_wstring(fileName).c_str(), 0, 0, "fx_4_0", shaderFlags, 0, device, 0, 0, &effect, &compilationErrors, 0);
		if(FAILED(hr))
		{
			if(compilationErrors)
			{
				MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
				ReleaseCOM(compilationErrors);
			}
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
		} 
	}


	void buildVertexLayouts()
	{
			//Create the vertex input layout
			D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
				{ "vPPos",    0, DXGI_FORMAT_R32G32B32_FLOAT,    1, 0,	 D3D10_INPUT_PER_INSTANCE_DATA, 1 },
			};

			//Create the input layout
			D3D10_PASS_DESC PassDesc;
			technique->GetPassByIndex(0)->GetDesc(&PassDesc);
			HR(device->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,PassDesc.IAInputSignatureSize, &layout));
	}
};




 

#endif