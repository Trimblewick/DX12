struct VS_INPUT
{
	float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvpMat);
    output.uv = input.uv;

	return output;
}