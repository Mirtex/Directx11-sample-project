#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "..\Common\DirectXHelper.h"
#include "..\Common\DDSTextureLoader.h"
#include "MeshLoader.h"

using namespace GX2_HWProject;
using namespace DirectX;
using namespace Windows::Foundation;


void Sample3DSceneRenderer::RenderStuff()
{
	m_constantBufferDataSky.modelID = 0;
	XMFLOAT4 loc = XMFLOAT4(camera._41, camera._42, camera._43, 0.0f);
	m_constantBufferData.cameraPos = loc;
	//skyBox
	XMFLOAT4 skylpos[5] = { XMFLOAT4(0,0,0,0) };

	XMFLOAT4 skylight[5] = { XMFLOAT4(0,0,0,0) };

	//Light stuff
	XMFLOAT4 lightDirections[] =
	{
		XMFLOAT4(1.3f, -0.2f, 0, 1.0f),
		XMFLOAT4(-3.560f, -.5f, 0, 1.0f),
		XMFLOAT4(2.3f, 0.0f, 0, 1.0f),
		XMFLOAT4(3.3f, 0.0f, 0, 1.0f),
		XMFLOAT4(4.3f, 0.0f, 0, 1.0f),
		XMFLOAT4(0,-2.5f, 0, 1.0f),					// Spot Light
		XMFLOAT4(0, 2.5f, 0, 1.0f),					// Spot Light
		XMFLOAT4(0.3f, 0.3f, 1.3f, 1.0f),				// Last Light is Directional
	};

	XMFLOAT4 lightColors[] =
	{
		// use one of these to move around
		XMFLOAT4(0.1f, 0.01f, 0.2f, 0.0f),
		XMFLOAT4(0.01f, 0.2f, 0.2f, 0.0f),
		XMFLOAT4(0.05f, 0.05f, 0.05f, 0.0f),		
		XMFLOAT4(0.05f, 0.05f, 0.05f, 0.0f),		
		XMFLOAT4(0.05f, 0.05f, 0.05f, 0.0f),		
		XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f),
		XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f)		// Last Light is Directional
	};

	//light rotation math here
	for (unsigned int i = 0; i < NUM_PTLIGHTS; ++i)  // Only for Spot Lights
	{
		XMMATRIX lightRotation = XMMatrixRotationY(-storedRadians*(1 + i));
		XMVECTOR lightDir = { 0 };
		lightDir = XMLoadFloat4(&lightDirections[i]);
		lightDir = XMVector3Transform(lightDir, lightRotation);
		XMStoreFloat4(&lightDirections[i], lightDir);

	}
	

	lightDirections[5].y += secondsPassed / 4.0f;
	lightDirections[6].y -= secondsPassed / 4.0f;
	lightDirections[7].x += secondsPassed;



	//set up lights
	for (unsigned int i = 0; i < (NUM_LIGHTS+1); i++)
	{
			m_constantBufferData.lightColor[i] =	 lightColors[i];
		m_constantBufferData.lightDirection[i] = lightDirections[i];

	}
	

	m_constantBufferData.modelID = ID_CCUBE;

	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	
	//skybox
	m_constantBufferDataSky = m_constantBufferData;
	XMStoreFloat4x4(&m_constantBufferDataSky.model[0], XMMatrixTranspose(XMMatrixScaling(100.0f, 100.0f, 100.0f) * XMMatrixTranslationFromVector(XMLoadFloat4(&loc))));


	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1( m_constantBuffer.Get(), 0,	NULL, &m_constantBufferData, 0,	0, 0);


	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(
		m_indexBufferCube.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	ID3D11ShaderResourceView* resViews[] = { m_cubeResView };
	context->PSSetShaderResources(0, 1, resViews);
	context->PSSetSamplers(0, 1, &m_cubeSampler.p);


	// Pixel shader constant buffer
	context->PSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Draw the objects.
	context->DrawIndexedInstanced(
		m_indexCountCube, 1,
		0,
		0,
		0
	);
	cubeModel = m_constantBufferData.model[0];
	//Reset modelID for now
	m_constantBufferData.modelID = 0;


	//Draw Loaded Mesh

	XMMATRIX holdMatrix = XMMatrixRotationX(-1.5708);
	holdMatrix = XMMatrixTranslation(0, 0, 1.5f) *holdMatrix;
	XMMATRIX holdScale = XMMatrixScaling(2.5f, 2.5f, 2.5f);
	holdMatrix = holdScale * holdMatrix;
	XMStoreFloat4x4(&m_constantBufferData.model[0], XMMatrixTranspose(holdMatrix));

	holdMatrix = XMMatrixRotationX(-4.7124);
	holdMatrix = XMMatrixTranslation(0, 0, 1.5f) *holdMatrix;
	holdScale = XMMatrixScaling(2.5f, 2.5f, 2.5f);
	holdMatrix = holdScale * holdMatrix;
	XMStoreFloat4x4(&m_constantBufferData.model[1], XMMatrixTranspose(holdMatrix));

	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBufferHold.GetAddressOf(),
		&stride,
		&offset
	);

	//Send hold texture
	resViews[0] = m_holdResView;
	context->PSSetShaderResources(0, 1, resViews);

	context->IASetIndexBuffer(
		m_indexBufferHold.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);


	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);
	context->DrawIndexedInstanced(m_indexCountHold, 2, 0, 0, 0);

	m_constantBufferDataSky.modelID = ID_SKYBOX;

	//Skybox
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferDataSky,
		0,
		0,
		0
	);

	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBufferSky.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	//	ID3D11ShaderResourceView* resViews[] = { m_cubeResView };
	resViews[0] = m_skyResView;
	context->PSSetShaderResources(1, 1, resViews);
	context->PSSetSamplers(1, 1, &m_cubeSampler.p);

	// Pixel shader constant buffer
	context->PSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Draw the objects.
	context->DrawIndexed(
		m_indexCountCube,
		0,
		0
	);

	//		context->RSSetViewports(1, &m_minimapViewport);

	m_constantBufferData.modelID = ID_PRISM;
	float blendFactor[] = { 0.5f, 0.5f, 0.5f, 0.5f };

	context->OMSetBlendState(bState, NULL, 0xffffffff);

	resViews[0] = m_prismResView;
	//Draw Light source(s)
	context->PSSetShaderResources(0, 1, resViews);

	XMVECTOR camL = XMLoadFloat4(&m_constantBufferData.cameraPos);
	XMVECTOR prismL1 = XMLoadFloat4(&m_constantBufferData.lightDirection[2]);
	XMVECTOR prismL2 = XMLoadFloat4(&m_constantBufferData.lightDirection[3]);
	XMVECTOR prismL3 = XMLoadFloat4(&m_constantBufferData.lightDirection[4]);
	camL = XMVector4Dot(camL, camL);
	prismL1 = XMVector4Dot(prismL1, prismL1);
	prismL2 = XMVector4Dot(prismL2, prismL2);
	prismL3 = XMVector4Dot(prismL3, prismL3);
	prismL1 = camL - prismL1;
	prismL2 = camL - prismL2;
	prismL3 = camL - prismL3;

	XMFLOAT4 p1, p2, p3;
	XMFLOAT4 temp;

	XMStoreFloat4(&p1, prismL1);
	XMStoreFloat4(&p2, prismL2);
	XMStoreFloat4(&p3, prismL3);
	p1.x = abs(p1.x);
	p2.x = abs(p2.x);
	p3.x = abs(p3.x);

		
	for (unsigned int i = 0; i < NUM_LIGHTS; ++i)
	{
		if (i < 2 || i >= 5)
		{
			XMMATRIX lightMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&lightDirections[i]));
			XMMATRIX lightScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			lightMatrix = lightScale * lightMatrix;
			lightMatrix = XMMatrixRotationY(-storedRadians) * lightMatrix;

			XMStoreFloat4x4(&m_constantBufferData.model[0], XMMatrixTranspose(lightMatrix));
			context->IASetVertexBuffers(
				0,
				1,
				m_vertexBufferPrism.GetAddressOf(),
				&stride,
				&offset
			);


			context->UpdateSubresource1(
				m_constantBuffer.Get(),
				0,
				NULL,
				&m_constantBufferData,
				0,
				0,
				0
			);

		}
		else
		{
			unsigned int j;
			if (p1.x < p3.x)
			{
				i == 2 ? j = 4 : (i == 3 ? j = 3 : j = 2);
			}
			else
			{
				j = i;
			}

				XMMATRIX lightMatrix = XMMatrixTranslationFromVector(XMLoadFloat4(&lightDirections[j]));
				XMMATRIX lightScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
				lightMatrix = lightScale * lightMatrix;
				lightMatrix = XMMatrixRotationY(-storedRadians) * lightMatrix;

				XMStoreFloat4x4(&m_constantBufferData.model[0], XMMatrixTranspose(lightMatrix));
				context->IASetVertexBuffers(
					0,
					1,
					m_vertexBufferPrism.GetAddressOf(),
					&stride,
					&offset
				);


				context->UpdateSubresource1(
					m_constantBuffer.Get(),
					0,
					NULL,
					&m_constantBufferData,
					0,
					0,
					0
				);

		}
		context->Draw(24, 0);



	}

	m_constantBufferData.model[0] = cubeModel;


}

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCountHold(0),
	m_indexCountCube(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	
	static const XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));

	//skyBox
	m_constantBufferDataSky.view = m_constantBufferData.view;

}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 60.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);
		
	m_constantBufferDataSky.projection = m_constantBufferData.projection;

}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;

#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;

extern char buttons[256];

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
		
		
		if (secondsPassed < 4.0f && !flipped)
		{
			secondsPassed += timer.GetElapsedSeconds();
		}
		else if (secondsPassed > 0 && flipped)
		{
			secondsPassed -= timer.GetElapsedSeconds();
		}

		if(secondsPassed >= 4.0f && !flipped || secondsPassed <= 0 && flipped)
		{
			flipped = !flipped;
		}

		Rotate(radians);
		
	}

	XMMATRIX newcamera = XMLoadFloat4x4(&camera);
	//m_constantBufferDataSky.view = camera;

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (buttons['A'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (s_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * timer.GetElapsedSeconds() * 5.0;
	}

	if (d_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
	}

	Windows::UI::Input::PointerPoint^ point = nullptr;

	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newcamera = XMMatrixRotationX(-diffy*0.01f) * newcamera * XMMatrixRotationY(-diffx*0.01f);
			newcamera.r[3] = pos;
		}
	}

	XMStoreFloat4x4(&camera, newcamera);

	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));


	mouse_move = false;/*Reset*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model[0], XMMatrixTranspose(XMMatrixScaling(0.8f,0.8f,0.8f)*XMMatrixRotationY(radians)*XMMatrixRotationZ(radians)));
	m_constantBufferDataSky.storedRadians = m_constantBufferData.storedRadians = storedRadians = radians;
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	RenderStuff();
}

void Sample3DSceneRenderer::SetMinimapCamera()
{
	viewTemp = m_constantBufferData.view;
	projTemp = m_constantBufferData.projection;
	viewSkyTemp = m_constantBufferDataSky.view;
	projSkyTemp = m_constantBufferDataSky.projection;
	cameraLoc = camera;

	static const XMVECTORF32 eye = { 0.0f, 0.0f, -7.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -10.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));
	//XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	XMStoreFloat4x4(&m_constantBufferDataSky.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 60.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	m_constantBufferDataSky.projection = m_constantBufferData.projection;


}
void Sample3DSceneRenderer::ResetCamera()
{
	m_constantBufferData.view = viewTemp;
	m_constantBufferData.projection = projTemp;
	m_constantBufferDataSky.view = viewSkyTemp;
	m_constantBufferDataSky.projection = projSkyTemp;
	camera = cameraLoc;

}


void Sample3DSceneRenderer::CreateDeviceDependentResources()
{

	Size outputSize = m_deviceResources->GetOutputSize();
	m_minimapViewport = CD3D11_VIEWPORT(
		outputSize.Width * 0.75f,
		outputSize.Height * 0.75f,
		outputSize.Width,
		outputSize.Height

	);

	// Load shaders asynchronously.
	MeshLoader containerMesh;
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);
		
		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "INSTANCEID", 0, DXGI_FORMAT_R32_UINT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	CD3D11_BLEND_DESC1 blendDesc = CD3D11_BLEND_DESC1(CD3D11_DEFAULT());
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0;

	HRESULT res = m_deviceResources->GetD3DDevice()->CreateBlendState1(&blendDesc, &bState);

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});


	//Textures
	
	HRESULT result;
	result = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/ice_crust3.dds", (ID3D11Resource**)&m_holdTexture, &m_holdResView);

	result = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/CubeTexture.dds", (ID3D11Resource**)&m_cubeTexture, &m_cubeResView);
	
	result = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Box_glass.dds", (ID3D11Resource**)&m_prismTexture, &m_prismResView);
	
	result = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/OutputCube3.dds", (ID3D11Resource**)&m_skyTexture, &m_skyResView);

	CD3D11_SAMPLER_DESC cubeSamplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	cubeSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	cubeSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	cubeSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	result = m_deviceResources->GetD3DDevice()->CreateSamplerState(&cubeSamplerDesc, &m_cubeSampler);
	

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this] () {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f), XMFLOAT3(0.f,0.f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(0.f,0.f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(0.f,0.f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f), XMFLOAT3(0.f,0.f, 1.0f)},

			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.f,1.5f, 1.0f), XMFLOAT3(0.f,0.f,-1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(0.f,0.f,-1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(0.f,0.f,-1.0f)},
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.f,1.5f, 1.0f), XMFLOAT3(0.f,0.f,-1.0f)},

			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.f,1.5f, 1.0f), XMFLOAT3(-1.0f,0.f,0.f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(-1.0f,0.f,0.f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(-1.0f,0.f,0.f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f), XMFLOAT3(-1.0f,0.f,0.f)},

			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f), XMFLOAT3(1.0f,0.f,0.f) },
			{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(1.0f,0.f,0.f) },
			{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(1.0f,0.f,0.f) },
			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.f,1.5f, 1.0f), XMFLOAT3( 1.0f,0.f,0.f)},

			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.f,1.5f, 1.0f), XMFLOAT3(0.f, 1.0f,0.f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(0.f, 1.0f,0.f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(0.f, 1.0f,0.f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.f,1.5f, 1.0f), XMFLOAT3(0.f, 1.0f,0.f)},
																	
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.f,1.f, 1.0f), XMFLOAT3(0.f,-1.0f,0.f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.f,0.f, 1.0f), XMFLOAT3(0.f,-1.0f,0.f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.f,0.f, 1.0f), XMFLOAT3(0.f,-1.0f,0.f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.f,1.f, 1.0f), XMFLOAT3(0.f,-1.0f,0.f)},
		};
		
		static const VertexPositionColor prismVertices[] =
		{
			{ XMFLOAT3(-0.25f, 0, 0.25f),	XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },
			{ XMFLOAT3(     0,  0.5f,  0),	XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },
			{ XMFLOAT3( 0.25f, 0, 0.25f),   XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },
			
			{ XMFLOAT3(		0, -0.5f,  0),	XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },
			{ XMFLOAT3(-0.25f, 0, 0.25f),	XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },
			{ XMFLOAT3( 0.25f, 0, 0.25f),   XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f))) },

			{ XMFLOAT3( 0.25f, 0, 0.25f),	XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3(0,  0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3( 0.25f, 0, -0.25f),  XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f)), 0.0f) },

			{ XMFLOAT3(0, -0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3( 0.25f, 0, 0.25f),	XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3( 0.25f, 0, -0.25f),  XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3((1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },

			{ XMFLOAT3( 0.25f, 0,  -0.25f),	XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f))) },
			{ XMFLOAT3(0,  0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f))) },
			{ XMFLOAT3(-0.25f, 0,  -0.25f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (1.0f / sqrt(2.0f)), ( 1.0f / sqrt(2.0f))) },

			{ XMFLOAT3(0, -0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f))) },
			{ XMFLOAT3( 0.25f, 0,  -0.25f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f))) },
			{ XMFLOAT3(-0.25f, 0,  -0.25f),	XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, (-1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f))) },

			{ XMFLOAT3(-0.25f, 0,  -0.25f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3(0,  0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), (1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3(-0.25f, 0,  0.25f),	XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), ( 1.0f / sqrt(2.0f)), 0.0f) },

			{ XMFLOAT3(0, -0.5f,  0),		XMFLOAT3(0.5f, 0.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3(-0.25f, 0, -0.25f),  XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },
			{ XMFLOAT3(-0.25f, 0,  0.25f),	XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3((-1.0f / sqrt(2.0f)), (-1.0f / sqrt(2.0f)), 0.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		D3D11_SUBRESOURCE_DATA vertexBufferDataPrism = { 0 };
		vertexBufferDataPrism.pSysMem = prismVertices;
		vertexBufferDataPrism.SysMemPitch = 0;
		vertexBufferDataPrism.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDescPrism(sizeof(prismVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDescPrism,
				&vertexBufferDataPrism,
				&m_vertexBufferPrism
			)
		);

		MeshLoader CosmicHold;
		CosmicHold.LoadMeshFromFile("Assets/CosmicHolder.obj");
		
		numHoldVerts = CosmicHold.fullVertex.size();


		D3D11_SUBRESOURCE_DATA vertexBufferDataHold = { 0 };
		vertexBufferDataHold.pSysMem = &CosmicHold.fullVertex[0];
		vertexBufferDataHold.SysMemPitch = 0;
		vertexBufferDataHold.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDescHold(sizeof(VertexPositionColor)*numHoldVerts, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDescHold,
				&vertexBufferDataHold,
				&m_vertexBufferHold
			)
		);

		m_indexCountHold = CosmicHold.vIndices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = &CosmicHold.vIndices[0];
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int)*m_indexCountHold, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBufferHold
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices [] =
		{

			0, 1, 2, // -z
			0, 2, 3,

			4, 5, 6, // +z
			4, 6, 7,

			8, 9, 10, // -x
			8, 10,11,

			12,13,14, // +x
			12,14,15,

			16,17,18, // +y
			16,18,19,

			20,21,22, // -y
			20,22,23,
			
		};
		//CosmicHold.LoadMeshFromFile(path);
		m_indexCountCube = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferDataCube = {0};
		indexBufferDataCube.pSysMem = cubeIndices;
		indexBufferDataCube.SysMemPitch = 0;
		indexBufferDataCube.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDescCube(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDescCube,
				&indexBufferDataCube,
				&m_indexBufferCube
				)
			);

		static const unsigned short skyBoxIndices[] =
		{

			2, 1, 0, // -z
			3, 2, 0,

			6, 5, 4, // +z
			7, 6, 4,

			10,9, 8,  // -x
			11,10,8,

			14,13,12, // +x
			15,14,12,

			18,17,16, // +y
			19,18,16,

			22,21,20, // -y
			23,22,20,

		};

		D3D11_SUBRESOURCE_DATA indexBufferDataSky = { 0 };
		indexBufferDataSky.pSysMem = skyBoxIndices;
		indexBufferDataSky.SysMemPitch = 0;
		indexBufferDataSky.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDescSky(sizeof(skyBoxIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDescSky,
				&indexBufferDataSky,
				&m_indexBufferSky
				)
		);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this] () {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBufferCube.Reset();
	m_indexBufferHold.Reset();
	//Added code
	m_vertexBufferPrism.Reset();
	m_vertexBufferHold.Reset();
	
}