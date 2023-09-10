struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	
	depthValue = input.depthPosition.z / input.depthPosition.w;

	if (depthValue < 1.0f)
	{
		color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	if (depthValue > 0.9f)
	{
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	if (depthValue > 0.925f)
	{
		color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	return color;
}