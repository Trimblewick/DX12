SamplerState cubeSamp : register(s0);
TextureCube cubeMap : register(t0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 uvw : POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return cubeMap.Sample(cubeSamp, input.uvw);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
}