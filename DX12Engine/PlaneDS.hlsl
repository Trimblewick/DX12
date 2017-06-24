Texture2D heightMap : register(t1);
SamplerState heightSamp : register(s1);

cbuffer MatrixBuffer : register(b0)
{
	float4x4 wvpMat;
	float padding[48];
};

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
	float2 uv : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : POSITION;
	float2 uv : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

    

    float2 uvs = patch[0].uv * domain.x + patch[1].uv * domain.y + patch[2].uv * domain.z;
    Output.uv = uvs;
    uvs *= 0.01f;
    float hs = heightMap.SampleLevel(heightSamp, uvs, 0).r;

    float3 pos = patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z;
    pos.y += hs * 100;
    Output.vPosition = mul(float4(pos, 1), wvpMat);


	return Output;
}
