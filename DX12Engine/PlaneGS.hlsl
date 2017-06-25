struct GS_INPUT
{
    float4 vPosition : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct GS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

[maxvertexcount(3)]
void main(
	triangle GS_INPUT input[3], 
	inout TriangleStream<GS_OUTPUT> output)
{
    float4 center = (input[0].vPosition + input[1].vPosition + input[2].vPosition) / 3.0f;
    float3 v1 = input[0].vPosition - input[1].vPosition;
    float3 v2 = input[0].vPosition - input[2].vPosition;

    float3 upDir = cross(v1, v2);
    
    if (0.0f > dot(upDir, float3(0, 1, 0)))
    {
        upDir = -upDir;
    }

	for (uint i = 0; i < 3; i++)
	{
        GS_OUTPUT element;
        element.vPosition = input[i].vPosition;
        float3 offs = input[i].vPosition.xyz - center.xyz;
        element.normal = normalize(upDir + offs);
        element.uv = input[i].uv;
		output.Append(element);
	}
}