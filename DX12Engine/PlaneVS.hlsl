struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};

VS_OUTPUT main( VS_INPUT input ) : SV_POSITION
{
	VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvpMat);

	return output;
}