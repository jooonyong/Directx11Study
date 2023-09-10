Texture2D shaderTextures[3];
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 lightDirection;
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float4 specularIntensity;
	float3 reflection;
	float4 specular;

	textureColor = shaderTextures[0].Sample(SampleType, input.tex);

	bumpMap = shaderTextures[1].Sample(SampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	bumpNormal = normalize(bumpNormal);

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(bumpNormal, lightDir));

	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	if (lightIntensity > 0.0f)
	{
		specularIntensity = shaderTextures[2].Sample(SampleType, input.tex);
		reflection = normalize(2 * lightIntensity * bumpNormal - lightDir);
		
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
		specular = specular * specularIntensity;
		
		color = saturate(color + specular);
	}

	return color;
}
