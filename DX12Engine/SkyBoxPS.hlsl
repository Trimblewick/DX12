SamplerState cubeSamp : register(s0);
Texture2D cubeMap : register(t0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return cubeMap.Sample(cubeSamp, input.uv);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
}