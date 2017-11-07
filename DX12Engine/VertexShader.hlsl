struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal: NORMAL;
};

struct VS_OUTPUT
{
    float4 position: SV_POSITION;
    float4 normal: NORMAL;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
	float padding[48];
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(input.position, wvpMat);
    output.normal = input.normal;
    return output;
}