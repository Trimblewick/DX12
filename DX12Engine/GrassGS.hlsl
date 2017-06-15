struct GS_INPUT
{
    float3 position : POSITION;
};

struct GS_OUTPUT
{
	float4 position : SV_POSITION;
};


cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};

[maxvertexcount(18)]
void main(
	triangle GS_INPUT input[3], 
	inout TriangleStream<GS_OUTPUT> output
)
{
    GS_OUTPUT element;
    
    
	for (uint i = 0; i < 2; i++)
	{
        float4 p = mul(float4(input[i].position, 1), wvpMat);
        float4 p2 = mul(float4(input[i + 1].position, 1), wvpMat);
        element.position = p + float4(0.05f, 0.0f, 0.0f, 0.0f);
        output.Append(element);
        element.position = p + float4(-0.05f, 0.0f, 0.0f, 0.0f);
		output.Append(element);
        element.position = p + float4(0.05f, p2.y - p.y, 0.0f, 0.0f);
        output.Append(element);
        output.RestartStrip();
        element.position = p + float4(0.05f, 0.0f, 0.0f, 0.0f);
        output.Append(element);
        element.position = p + float4(0.05f, p2.y - p.y, 0.0f, 0.0f);
        output.Append(element);
        element.position = p + float4(-0.05f, p2.y - p.y, 0.0f, 0.0f);
        output.Append(element);
        output.RestartStrip();
    }
    
}