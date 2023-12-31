Texture2D shaderTextures[2];
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInputType input) :SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 blendColor;

	color1 = shaderTextures[0].Sample(SampleType, input.tex);
	color2 = shaderTextures[1].Sample(SampleType, input.tex);

	blendColor = color1 * color2 * 2.0;

	blendColor = saturate(blendColor);

	return blendColor;
}