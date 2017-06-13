
struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 postion : SV_POSITION;
};

cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};

VS_OUTPUT main( VS_INPUT input )
{
    VS_OUTPUT output;
    output.postion = mul(float4(input.position, 1.0f), wvpMat);
	return output;
}