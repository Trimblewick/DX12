
cbuffer MatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 wvpMat;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 uvw : POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvpMat).xyww;
    output.uvw = mul(input.position, (float3x3)worldMatrix);
	return output;
}