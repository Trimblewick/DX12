


struct VS_INPUT
{
	float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float3 position : POSITION;
    float2 uv : TEXCOORD;
};


VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;
	output.position = input.position;
    output.uv = input.uv;

	return output;
}