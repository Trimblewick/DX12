
struct VS_INPUT
{
    float3 position : POSITION;
};

/*struct VS_OUTPUT
{
    float3 postion : POSITION;
};*/


VS_INPUT main( VS_INPUT input )
{
    //VS_OUTPUT output;
    //output.postion = mul(float4(input.position, 1.0f), wvpMat);
	return input;
}