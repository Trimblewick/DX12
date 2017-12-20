cbuffer CameraBuffer : register(b0)
{
	float4x4 vpMatrix;
	float3 position;
	float pad1;
	float3 forward;
	float pad2;
	float3 right;
	float pad3;
	float3 up;
	float pad4;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

RWTexture2D<float4> target : register(u0);

cbuffer rootConstants : register(b1)
{
    uint bufferIndex;
};



[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (bufferIndex == 0)
        target[DTid.xy] = float4(0, 1, 0, 1);
    if (bufferIndex == 1)
        target[DTid.xy] = float4(1, 0, 0, 1);
    if (bufferIndex == 2)
        target[DTid.xy] = float4(0, 0, 1, 1);

}