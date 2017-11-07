
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
};


float4 main( PS_INPUT input ) : SV_TARGET
{
	return float4(0.75f, 0.75f, 0.75f, 1.0f);
}