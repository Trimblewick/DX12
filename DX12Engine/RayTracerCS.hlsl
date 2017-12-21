cbuffer CameraBuffer : register(b0)
{
	float4x4 vpMatrix;
	float3 camPosition;
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
    uint screenHeight;
    uint screenWidth;
};

struct Triangle
{
    float4 p1;
    float4 p2;
    float4 p3;
};

bool RayTriangleIntersection(float3 rayDir, float3 rayPos, Triangle instance)
{
    float3 normal = cross(instance.p2.xyz - instance.p1.xyz, instance.p3.xyz - instance.p1.xyz);

    double s = dot(normal, rayDir);
    if (s * s < 0.0000000000001)//almost parallel, might cause aliasing?
        return false;


    float d = dot(normal, instance.p1.xyz);
    float t = (d + dot(rayPos, normal)) / s;

    if (t < 0)
        return false;//behind camera


    float3 intersectionPoint = rayPos + rayDir * t;//itnersection point in the plane, though it might not be inside the triangle

    if (dot(normal, cross(instance.p2.xyz - instance.p1.xyz, intersectionPoint - instance.p1.xyz)) < 0)
        return false;
    if (dot(normal, cross(instance.p1.xyz - instance.p3.xyz, intersectionPoint - instance.p3.xyz)) < 0)
        return false;
    if (dot(normal, cross(instance.p3.xyz - instance.p2.xyz, intersectionPoint - instance.p2.xyz)) < 0)
        return false;

    return true;
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Triangle anInstance;
    anInstance.p1 = mul(float4(-1.5f, -1.5f, 1.5f, 1), vpMatrix);
    anInstance.p3 = mul(float4(0.0f, 1.5f, -1.5f, 1), vpMatrix);
    anInstance.p2 = mul(float4(0.5f, -0.5f, 0.0f, 1), vpMatrix);
    
    float3 pixelPosition = mul(float4((DTid.x * 2.0f - (float) screenWidth + 1.0f) / (float) screenWidth, (DTid.y * 2.0f - (float) screenHeight + 1.0f) / (float) screenHeight, 3.0f, 1.0f), vpMatrix).xyz;
    
    //float4 temp = mul(float4(0, 0, 0, 1), vpMatrix);  //alternative way of calculating camPosition. though it is inclided in my buffer
    //float3 camPos = temp.xyz / temp.w;

    float3 ray = normalize(pixelPosition - camPosition);
    
    if (RayTriangleIntersection(ray, pixelPosition, anInstance))
        target[DTid.xy] = float4(0.5f, 0.2f, 0.1f, 1);
    else
        target[DTid.xy] = float4(0.5f, 0.5f, 0.5f, 1);

    //if (bufferIndex == 0)
        
    //if (bufferIndex == 1)
      //  target[DTid.xy] = float4(1, 0, 0, 0);
    //if (bufferIndex == 2)
      //  target[DTid.xy] = float4(0, 0, 1, 0);

}