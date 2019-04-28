#pragma once
using namespace DirectX;

namespace GX2_HWProject
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	__declspec(align(16)) struct ModelViewProjectionConstantBuffer
	{
		XMFLOAT4X4 model[2];
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4 lightDirection[8];
		XMFLOAT4 lightColor[8];
		unsigned int modelID;
		float storedRadians;
		XMFLOAT4 cameraPos;


	};

	// Used to send per-vertex data to the vertex shader.
	__declspec(align(16)) struct VertexPositionColor
	{
		XMFLOAT3 pos;
		XMFLOAT3 color;
		XMFLOAT3 norm;

 	};
}