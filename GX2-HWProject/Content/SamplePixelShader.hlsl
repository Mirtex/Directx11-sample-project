
texture2D holdTexture : register(t0);
textureCUBE skyTexture : register(t1);

SamplerState holdSampler : register(s0);
SamplerState skySampler : register(s1);

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model[2];
	matrix view;
	matrix projection;
	float4 lightDirection[8];
	float4 lightColor[8];
	unsigned int modelID;
	float storedRadians;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLDPOS;
	float3 viewDir : VIEWDIR;
	float3 color : COLOR0;
	float3 norm : TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 pixelColor;
	float3 intensity, reflection, specular = 0;
	bool flip = false;
	if (modelID != 2)
	{
		pixelColor = holdTexture.Sample(holdSampler, input.color);
		
		if (modelID != 3)
		{
			if(pixelColor.x >= 0.5f && pixelColor.y >= 0.5f && pixelColor.z >= 0.5f)
				pixelColor *= float4(1.5f,1.5f,1.5f,1.2f);

		}
		else
		{
			if (pixelColor.x >= 0.2f && pixelColor.y >= 0.2f && pixelColor.z >= 0.2f)
				pixelColor *= float4(1.4f, 1.4f, 1.8f, 1.4f);

		}

		float3 tempDirection = 0;
		float surfaceRatio = 0;
		float3 coneDir = float3(0,-1.0f, 0);
		float spotFactor;
		float coneRatio = 0.98f;
		float specularIntensity = 100.0f;
		float4 energyType = float4(1.0f, 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < 8; i++)
		{
			//if (i == 1)
			//	continue111111;
			spotFactor = 1.0f;
			if (i < 5)
			{
				tempDirection = normalize(lightDirection[i].xyz - input.worldPos.xyz);  // Point Light

			}
			else if (i < 7 && i >= 5)
			{
				coneDir.y *= -1.0f;
				tempDirection = normalize(lightDirection[i].xyz - input.worldPos.xyz);  // Spot Light
				surfaceRatio = saturate(dot(-tempDirection, coneDir));
				spotFactor = (surfaceRatio > coneRatio) ? 1.0f : 0;
				if (modelID == 1 && spotFactor != 0)
				{
					energyType.x *= 1.1f;
					energyType.z *= 1.1f;
				}
				//coneDir.y = coneDir.y * -1.0f;
			}
			else
			{
				tempDirection = normalize(lightDirection[i]);  // Directional Light

			}
			
			intensity = saturate(dot(tempDirection, input.norm));
			reflection = normalize(2.0f * intensity*input.norm - tempDirection);
			specular = pow(saturate(dot(reflection, input.viewDir)), specularIntensity);
			pixelColor += (float4(intensity, 0.0f)*lightColor[i])*spotFactor;
			if (modelID == 1 )
			{
				pixelColor*= energyType;
			}
			
		}

		if (pixelColor.z > 0.3f && pixelColor.y < 0.15f && pixelColor.x < 0.15f && modelID != 1)
		{
			pixelColor.x *= 5.0f;
			pixelColor.y *= 5.0f;
			pixelColor.z *= 5.0f;
		}
	}
	else
	{
		pixelColor = skyTexture.Sample(skySampler, input.worldPos.xyz);
	}
	pixelColor += float4(specular, 0.0f);
	if (modelID == 3)
	{
		pixelColor.a = 0.5f;
	}
	return pixelColor;
	//return pixelColor;
}
