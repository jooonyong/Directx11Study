Texture2D shaderTextures[2];
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = shaderTextures[0].Sample(SampleType, input.tex);
	bumpMap = shaderTextures[1].Sample(SampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
	bumpNormal = normalize(bumpNormal);

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(bumpNormal, lightDir));

	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}