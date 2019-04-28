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
	float4 cameraPos;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
	float3 norm : NORMAL;
	unsigned int instID : SV_INSTANCEID;
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

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	
	//if(instID == 0)
	output.viewDir = mul(input.pos, model[input.instID]);
	output.viewDir = normalize(cameraPos - output.viewDir);
	// Transform the vertex position into projected space.
	if (modelID == 2)
	{
		output.worldPos = pos;
		pos = mul(pos, model[input.instID]);
		pos = mul(pos, view);
		pos = mul(pos, projection);
		output.pos = pos;
	}
	else
	{
		pos = mul(pos, model[input.instID]);
		output.worldPos = pos;
		pos = mul(pos, view);
		pos = mul(pos, projection);
		output.pos = pos;
	}
	
	// Pass the color (UVs) through without modification.

	if (modelID == 1) // Cube
	{
		float x = input.color.x*cos(storedRadians);
		x += input.color.y*sin(storedRadians);
		float y = input.color.x*-sin(storedRadians);
		y += input.color.y*cos(storedRadians);
		input.color = float3(x, y, 0.0f);
	}

	output.color = input.color;
	output.norm = mul(float4(input.norm, 0.0f), model[input.instID]).xyz;
	return output;
}
