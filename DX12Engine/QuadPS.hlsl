Texture2D BackTex : register(t0);
SamplerState StaticSamp : register(s0);

struct PS_INPUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return BackTex.Sample(StaticSamp, input.uv);
}