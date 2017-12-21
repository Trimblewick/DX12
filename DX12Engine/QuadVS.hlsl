struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
	VS_OUTPUT output;

    output.pos = float4((id == 2 || id == 3) * 2 - 1, (id == 1 || id == 3) * 2 - 1, 0, 1);
	output.uv = float2((id == 2 || id == 3), (id == 0 || id == 2));

	return output;
}