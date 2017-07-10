
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 lightDir = normalize(float4(-1, -1, -1, 0));
    



	return float4(0.0f, 0.7f, 0.0f, 1.0f) + float4(0.3, 0.3, 0.0f, 0.0f)* dot(input.normal, lightDir) * 0.1f;
}