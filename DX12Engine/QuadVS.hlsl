struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
	VS_OUTPUT output;

    /*if (id == 0)
        output.pos = float4(-0.5, -0.5, 0, 1);
    if (id == 1)
        output.pos = float4(0, 0.5, 0, 1);
    if (id == 2)
        output.pos = float4(0.5, -0.5, 0, 1);
    if (id == 3)
        output.pos = float4(1, 1, 0, 1);

    if (id == 0)
        output.uv = float2(0, 0);
    if (id == 1)
        output.uv = float2(1, 0);
    if (id == 2)
        output.uv = float2(0, 1);
    if (id == 3)
        output.uv = float2(1, 1);*/

    output.pos = float4((id == 2 || id == 3) * 2 - 1, (id == 1 || id == 3) * 2 - 1, 0, 1);
	output.uv = float2((id == 2 || id == 3), (id == 0 || id == 2));

	return output;
}