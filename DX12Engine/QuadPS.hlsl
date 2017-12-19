Texture2D BackTex : register(t0);
SamplerState StaticSamp;

struct PS_INPUT
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return BackTex.Sample(StaticSamp, input.uv);
}