SamplerState heightSamp : register(s0);
Texture2D heightMap : register(t0);

cbuffer grassInstanceBuffer : register(b1)
{
    uint xuv;
    uint zuv;
    uint instanceI;//from what index to use the pregenerated staws
    uint width;//for uvs
}

struct GS_INPUT
{
    //float3 position : POSITION;
    uint index : INDEX;
};

struct GS_OUTPUT
{
	float4 position : SV_POSITION;
};

struct StructGrass
{
    float4 position[4];
    float4 binormal;
    float4 seed;
};

RWStructuredBuffer<StructGrass> patch : register(u0);

cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};

[maxvertexcount(128)]
void main(
	point GS_INPUT input[1], 
	inout TriangleStream<GS_OUTPUT> output
)
{
    uint index = input[0].index + instanceI;

    float4 binormalWS = mul(patch[index].binormal, wvpMat);

    GS_OUTPUT element;

    
    
    float2 uv = float2((patch[index].position[0].x + xuv) / (float) width, (patch[index].position[0].z + zuv) / (float) width);
    float yval = heightMap.SampleLevel(heightSamp, uv, 0);
    float4 hOffs = float4(xuv, yval * 100.0f, zuv, 0.0f);//mother of god, this is hardcoded af
    float4 wp[4];
    for (int i = 0; i < 4; ++i)
    {
        wp[i] = mul(patch[index].position[i] + hOffs, wvpMat);
    }

    for (int i = 0; i < 3; ++i)
    {
        element.position = wp[i] + patch[index].seed[i] * binormalWS;
        output.Append(element);
        element.position = wp[i + 1] + patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        element.position = wp[i] - patch[index].seed[i] * binormalWS;
        output.Append(element);
        output.RestartStrip();
        element.position = wp[i + 1] + patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        element.position = wp[i] - patch[index].seed[i] * binormalWS;
        output.Append(element);
        element.position = wp[i + 1] - patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        output.RestartStrip();
        element.position = wp[i] + patch[index].seed[i] * binormalWS;
        output.Append(element);
        element.position = wp[i] - patch[index].seed[i] * binormalWS;
        output.Append(element);
        element.position = wp[i + 1] + patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        output.RestartStrip();
        element.position = wp[i + 1] + patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        element.position = wp[i + 1] - patch[index].seed[i + 1] * binormalWS;
        output.Append(element);
        element.position = wp[i] - patch[index].seed[i] * binormalWS;
        output.Append(element);
        output.RestartStrip();
    }
}