Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // return interpolated color
    return t1.Sample(s1, input.texCoord);
}