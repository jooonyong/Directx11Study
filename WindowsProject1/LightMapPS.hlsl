Texture2D shaderTextures[2];
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) :SV_TARGET
{
	float4 color;
	float4 lightColor;
	float4 finalColor;

	color = shaderTextures[0].Sample(SampleType, input.tex);

	lightColor = shaderTextures[1].Sample(SampleType, input.tex);

	finalColor = color * lightColor;

	return finalColor;
}