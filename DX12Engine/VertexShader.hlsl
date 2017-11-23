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

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position: SV_POSITION;
    float4 normal: NORMAL;
};

VS_OUTPUT main(VS_INPUT input, uint vid : sv_vertexid)
{
    VS_OUTPUT output;
    
    output.position = mul(float4(input.position, 1), vpMatrix);
    output.normal = float4(input.normal, 1);


    return output;
}