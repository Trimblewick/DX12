
struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    uint index : INDEX;
};


VS_OUTPUT main( /*VS_INPUT input*/uint id : SV_VertexID )
{
    VS_OUTPUT output;
    output.index = id;
    //output.postion = mul(float4(input.position, 1.0f), wvpMat);
	return output;
}