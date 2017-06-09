
struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT input ) : SV_POSITION
{
    matrix mat = matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), mat);

	return output;
}