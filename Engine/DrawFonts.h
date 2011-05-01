#ifndef DRAWFONTS_H
#define DRAWFONTS_H

#include "Utilities.h"
#include <sstream>

class DrawFonts
{
public:
	ID3D10Device* device;
public:
	ID3DX10Font* font_tf2_small;
	ID3DX10Font* font_tf2_medium;
	ID3DX10Font* font_tf2_large;

	wstring text_fps;


	DrawFonts()
	{
		ID3DX10Font* font_tf2_small=0;
		ID3DX10Font* font_tf2_medium=0;
		ID3DX10Font* font_tf2_large=0;

		text_fps = L"";
	};
	~DrawFonts()
	{
		ReleaseCOM(font_tf2_small);
		ReleaseCOM(font_tf2_medium);
		ReleaseCOM(font_tf2_large);
	};
	void init(ID3D10Device* device)
	{
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
		D3DX10CreateFontIndirect(device, &fontDesc, &font_tf2_small);
		fontDesc.Height = 24;
		D3DX10CreateFontIndirect(device, &fontDesc, &font_tf2_medium);
		fontDesc.Height = 45;
		D3DX10CreateFontIndirect(device, &fontDesc, &font_tf2_large);


		/*D3DX10CreateFont(device, 35, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,  DEFAULT_PITCH | FF_DONTCARE, TEXT("Helvetica"), &font_tf2_medium);*/
               
	};
	void update(float dt)
	{
		static int nrOfFrames=0;
		static float time_start = 0.0f;
		static float time_interval = 0.0f;
		time_interval+=dt;
		time_start+=dt;
		nrOfFrames++;
	
		static float fps = 0;
		static float mspf = 0;

		//Compute averages over one second period.
		if(time_interval>=1.0f)
		{
			fps = (float)nrOfFrames;
			mspf = 1000.0f/fps;

			//Reset for next average.
			nrOfFrames = 0;
			time_interval  -= 1.0f;
		}

		wostringstream s;
		s << L"FPS " << fps;
		s.precision(3);
		s << L"\n" << "MPF " << mspf;
		s << L"\n" << "Time " << (int)time_start;
		text_fps = s.str();
	};
	void drawFPS()
	{
		//Draw text
		RECT R = {5, 5, 0, 0};
		font_tf2_medium->DrawText(0, text_fps.c_str(), -1, &R, DT_NOCLIP, WHITE);

		/*wstring text = L"This is text!";
		RECT R = {0, 0, 0, 0};
		SetRectEmpty(&R);
		font_tf2_medium->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));*/
	};
};






#endif