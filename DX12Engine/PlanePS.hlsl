Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

cbuffer CameraPositionBuffer : register(b1)
{
    float3 cameraPostion;
};

struct PS_INPUT
{
    float4 vPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 light = float3(10, 10, 10);

    float3 dir = normalize(light - input.vPosition.xyz);

    float ang = dot(dir, input.normal);
    float4 shade = ang * float4(0.5, 0.5, 0.5, 1);
    // return interpolated color
    return t1.Sample(s1, input.uv);
    //+shade;
}