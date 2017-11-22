cbuffer CameraBuffer : register(b0)
{
    float4x4 vpMatrix;
    float3 position;
    float pad1;
    float3 forward;
    float pad2;
    float3 right;
    float pad3;
    float3 up;
    float pad4;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};



struct VS_OUTPUT
{
    float4 position: SV_POSITION;
    float4 normal: NORMAL;
};


/*
cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
	float padding[48];
};*/

VS_OUTPUT main(float3 position : POSITION, float3 normal : NORMAL)
{
    VS_OUTPUT output;

    output.position = mul(position, (float3x4) vpMatrix);
    output.normal = float4(normal, 1);
    return output;
}