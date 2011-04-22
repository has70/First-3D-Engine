#ifndef BOX_H
#define BOX_H

#include "Utilities.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXCOLOR   color;
};

struct OffPos
{
	D3DXVECTOR3 off;
};

class Box
{
private:
	int mNumVertices;
	int mNumFaces;
	ID3D10Effect* effect;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB[2];
	ID3D10Buffer* mIB;
public:
	Box()
	{
		mVB[0]=0;
		mVB[1]=0;
		mIB=0;
	};
	~Box()
	{
		ReleaseCOM(mVB[0]);
		ReleaseCOM(mVB[1]);
		ReleaseCOM(mIB);
	};

	void init(ID3D10Device* device, float scale)
	{
		md3dDevice = device;

		mNumVertices = 8;
		mNumFaces    = 12; // 2 per quad

		// Create vertex buffer
		Vertex vertices[] =
		{
			{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), WHITE},
			{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), BLACK},
			{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), RED},
			{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), GREEN},
			{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), BLUE},
			{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), YELLOW},
			{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), CYAN},
			{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), MAGENTA},
		};

		OffPos off[] =
		{
			{D3DXVECTOR3(1.0f, 1.0f, 1.0f)},
		/*	{D3DXVECTOR3(-1.0f, +1.0f, -1.0f)},
			{D3DXVECTOR3(+1.0f, +1.0f, -1.0f)},
			{D3DXVECTOR3(+1.0f, -1.0f, -1.0f)},
			{D3DXVECTOR3(-1.0f, -1.0f, +1.0f)},
			{D3DXVECTOR3(-1.0f, +1.0f, +1.0f)},
			{D3DXVECTOR3(+1.0f, +1.0f, +1.0f)},
			{D3DXVECTOR3(+1.0f, -1.0f, +1.0f)},*/

			/*{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f)},*/
		};

		// Scale the box.
		for(int i = 0; i < mNumVertices; ++i)
			vertices[i].pos *= scale;

		// Scale the box.
		for(int i = 0; i < mNumVertices; ++i)
			off[i].off *= 1.0f;

		//Vertexbuffer
		if(true)
		{
			D3D10_BUFFER_DESC vbd;
			vbd.Usage = D3D10_USAGE_IMMUTABLE;
			vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
			vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = vertices;
			HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB[0]));
		}

		//Offset buffer
		if(true)
		{
			D3D10_BUFFER_DESC vbd;
			vbd.Usage = D3D10_USAGE_IMMUTABLE;
			vbd.ByteWidth = sizeof(OffPos) * 1;
			vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = off;
			HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB[1]));
		}


		// Create the index buffer

		int indices[] = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3, 
			4, 3, 7
		};

		if(true)
		{
			D3D10_BUFFER_DESC ibd;
			ibd.Usage = D3D10_USAGE_IMMUTABLE;
			ibd.ByteWidth = sizeof(int) * mNumFaces*3;
			ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			ibd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = indices;
			HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
		}
	};
	void draw()
	{
		unsigned stride = sizeof(Vertex);
		unsigned offset = 0;
		md3dDevice->IASetVertexBuffers(0, 2, mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
	};
};

#endif