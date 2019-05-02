#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <atlbase.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace GX2_HWProject
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:

		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
		void SetMinimapCamera();
		void ResetCamera();


	private:
		void RenderStuff();
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferCube;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		//Skybox
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferSky;


		//My stuff
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferPrism;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferHold;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferHold;

		//Textures
		CComPtr<ID3D11Texture2D>					m_holdTexture;
		CComPtr<ID3D11ShaderResourceView>			m_holdResView;

		CComPtr<ID3D11Texture2D>					m_cubeTexture;
		CComPtr<ID3D11ShaderResourceView>			m_cubeResView;
		CComPtr<ID3D11SamplerState>					m_cubeSampler;

		CComPtr<ID3D11Texture2D>					m_prismTexture;
		CComPtr<ID3D11ShaderResourceView>			m_prismResView;

		CComPtr<ID3D11Texture2D>					m_skyTexture;
		CComPtr<ID3D11ShaderResourceView>			m_skyResView;

		CComPtr<ID3D11BlendState1>					bState;

		unsigned int								numHoldVerts = 0;
		float										storedRadians = 0;
		float										secondsPassed = 0;
		bool										flipped = false;
		uint32										m_indexCountCube;
		D3D11_VIEWPORT								m_minimapViewport;
		XMFLOAT4X4 viewTemp, projTemp, viewSkyTemp, projSkyTemp;



		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		ModelViewProjectionConstantBuffer	m_constantBufferDataSky;
		uint32	m_indexCountHold;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		
		XMFLOAT4X4 world, camera, proj,cameraLoc, cubeModel;
	};

}

