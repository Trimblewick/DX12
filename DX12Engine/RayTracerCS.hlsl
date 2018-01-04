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

struct Triangle
{
    float4 p1;
    float4 p2;
    float4 p3;
    float4 color;
};

struct Light
{
    float4 position;
    float4 color;
};

StructuredBuffer<Triangle> triangles : register(t1);
StructuredBuffer<Light> lights : register(t2);

cbuffer rootConstants : register(b1)
{
    uint iNumberOfTriangles;
    uint iNumberOfLights;
    uint iScreenHeight;
    uint iScreenWidth;
};

Triangle TriangleToWS(Triangle tri)
{
    tri.p1 = mul(tri.p1, vpMatrix);
    tri.p2 = mul(tri.p2, vpMatrix);
    tri.p3 = mul(tri.p3, vpMatrix);
    return tri;
}

struct Hit
{
    float3 hitPoint;
    float4 color;
    float3 normal;
};

Hit RayTriangleIntersection(float3 rayDir, float3 rayPos, Triangle tri, Hit previousHit)
{
    float3 normal = cross(tri.p2.xyz - tri.p1.xyz, tri.p3.xyz - tri.p1.xyz);

    double s = dot(normal, rayDir);
    if (s * s < 0.0000000000001)//almost parallel, might cause aliasing?
        return previousHit;


    float d = dot(normal, tri.p1.xyz);
    float t = (d + dot(rayPos, normal)) / s;

    if (t < 0)
        return previousHit; //behind camera


    float3 intersectionPoint = rayPos + rayDir * t;//itnersection point in the plane, though it might not be inside the triangle

    if (dot(normal, cross(tri.p2.xyz - tri.p1.xyz, intersectionPoint - tri.p1.xyz)) < 0)
        return previousHit;
    if (dot(normal, cross(tri.p1.xyz - tri.p3.xyz, intersectionPoint - tri.p3.xyz)) < 0)
        return previousHit;
    if (dot(normal, cross(tri.p3.xyz - tri.p2.xyz, intersectionPoint - tri.p2.xyz)) < 0)
        return previousHit;

    if (length(intersectionPoint - camPosition) < length(previousHit.hitPoint - camPosition))//better hit achived
    {
        previousHit.hitPoint = intersectionPoint;
        previousHit.normal = normal;
        previousHit.color = tri.color;
    }

    return previousHit;
}

float4 AccumulateLights(Hit hit)
{
    float4 c = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < )
        dot(normal, normalize(lights[0].position.xyz - intersectionPoint)) * lights[0].color;
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Hit previousHit;
    previousHit.color = float4(0.3f, 0.0f, 0.0f, 1.0f);
    previousHit.hitPoint = float3(1000, 1000, 1000);

    float3 pixelPosition = mul(float4((DTid.x * 2.0f - (float) iScreenWidth + 1.0f) / (float) iScreenWidth, (DTid.y * 2.0f - (float) iScreenHeight + 1.0f) / (float) iScreenHeight, 3.0f, 1.0f), vpMatrix).xyz;
    float3 ray = normalize(pixelPosition - camPosition);

    for (int i = 0; i < iNumberOfTriangles; ++i)
    {
        Triangle triWS = TriangleToWS(triangles[i]);
    
        
        previousHit = RayTriangleIntersection(ray, pixelPosition, triWS, previousHit);
    }

    
    target[DTid.xy] = previousHit.color;
  
}