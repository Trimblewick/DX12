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
    uint index = input[0].index;

    float4 binormalWS = mul(patch[index].binormal, wvpMat);

    GS_OUTPUT element;

    float4 wp[4]; 
    for (int i = 0; i < 4; ++i)
    {
        wp[i] = mul(patch[index].position[i], wvpMat);
    }
 /*       wp[1] = mul(patch[index].position[1], wvpMat);
    wp[2] = mul(patch[index].position[2], wvpMat);
    wp[3] = mul(patch[index].position[3], wvpMat);*/
    
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