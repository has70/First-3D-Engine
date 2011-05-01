#ifndef BOX_H
#define BOX_H

#include "Utilities.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXCOLOR   color;
};

struct Instance
{
	D3DXVECTOR3 offset;
};

class Box
{
private:
	int nrOfVertices;
	int nrOfFaces;
	int nrOfInstances;

	ID3D10Effect* effect;
	ID3D10Device* device;
	ID3D10Buffer* vertexBuffer[2];
	ID3D10Buffer* indexBuffer;
public:
	Box()
	{
		vertexBuffer[0]=0;
		vertexBuffer[1]=0;
		indexBuffer=0;
	};
	~Box()
	{
		ReleaseCOM(vertexBuffer[0]);
		ReleaseCOM(vertexBuffer[1]);
		ReleaseCOM(indexBuffer);
	};

	void init(ID3D10Device* _device, float scale)
	{
		//Save path to device (for later use)
		device = _device;


		//========
		//Vertices
		//========

		//Create vertices
		nrOfVertices = 8;
		nrOfFaces    = 12; //2 per quad
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
		for(int i=0; i<nrOfVertices; i++)
			vertices[i].pos *= scale;

		//Create buffer
		D3D10_BUFFER_DESC vbd;
		vbd.Usage = D3D10_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(Vertex)*nrOfVertices;
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vertexInit;
		vertexInit.pSysMem = vertices;
		HR(device->CreateBuffer(&vbd, &vertexInit, &vertexBuffer[0]));


		//=========
		//Instances
		//=========

		//Instances vertices
		nrOfInstances = 8;
		Instance instances[] =
		{
			
			{D3DXVECTOR3(-1.0f, +1.0f, -1.0f)},
			{D3DXVECTOR3(-1.0f, -1.0f, -1.0f)},
			{D3DXVECTOR3(+1.0f, +1.0f, -1.0f)},
			{D3DXVECTOR3(+1.0f, -1.0f, -1.0f)},
			{D3DXVECTOR3(-1.0f, +1.0f, +1.0f)},
			{D3DXVECTOR3(-1.0f, -1.0f, +1.0f)},
			{D3DXVECTOR3(+1.0f, +1.0f, +1.0f)},
			{D3DXVECTOR3(+1.0f, -1.0f, +1.0f)},
		};
		for(int i=0; i<nrOfInstances; i++)
			instances[i].offset *= 1.3f;

		//Create buffer
		if(true)
		{
		D3D10_BUFFER_DESC ibd;
		ibd.Usage = D3D10_USAGE_DYNAMIC;
		ibd.ByteWidth = sizeof(Instance)*nrOfInstances;
		ibd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		ibd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA instanceInit;
		instanceInit.pSysMem = instances;
		HR(device->CreateBuffer(&ibd, &instanceInit, &vertexBuffer[1]));
		}


		//=====
		//Index
		//=====

		//Index buffer
		int indices[] = 
		{
			//Front
			0, 1, 2,
			0, 2, 3,
			//Back
			4, 6, 5,
			4, 7, 6,
			//Left
			4, 5, 1,
			4, 1, 0,
			//Right
			3, 2, 6,
			3, 6, 7,
			//Top
			1, 5, 6,
			1, 6, 2,
			//Bottom
			4, 0, 3, 
			4, 3, 7
		};


		////Create buffer
		//D3D10_BUFFER_DESC ixbd;
		//ixbd.Usage = D3D10_USAGE_IMMUTABLE;
		//ixbd.ByteWidth = sizeof(int)*nrOfFaces*3;
		//ixbd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		//ixbd.CPUAccessFlags = 0;
		//ixbd.MiscFlags = 0;
		//D3D10_SUBRESOURCE_DATA indexInit;
		//instanceInit.pSysMem = indices;
		//HR(device->CreateBuffer(&ixbd, &indexInit, &indexBuffer));

		if(true)
		{
			D3D10_BUFFER_DESC ibd;
			ibd.Usage = D3D10_USAGE_IMMUTABLE;
			ibd.ByteWidth = sizeof(int) * nrOfFaces*3;
			ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			ibd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = indices;
			HR(device->CreateBuffer(&ibd, &iinitData, &indexBuffer));
		}
	};
	void draw()
	{
		UINT strides[2] = {sizeof(Vertex),sizeof(Instance)};
		UINT offsets[2] = {0,0};

		device->IASetVertexBuffers(0, 2, vertexBuffer, strides, offsets);
		device->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		device->DrawIndexedInstanced(nrOfFaces*3,8,0,0,0);
	};
};

#endif

//if(true)
//{
//	D3D10_BUFFER_DESC ibd;
//	ibd.Usage = D3D10_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(int) * mNumFaces*3;
//	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D10_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = indices;
//	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
//}