cbuffer CameraBuffer : register(b0)
{
	float4x4 vpMatrix;
	float3 camPositionpp;
	float pad1;
	float3 forward;
	float pad2;
	float3 right;
	float pad3;
	float3 up;
	float pad4;
    float4x4 worldMatrix; //viewMatrix;
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

struct Hit
{
    float3 hitPoint;
    float padding;
    float4 color;
    float3 normal;
    float pad2;
};

float RayVsTriangle(float3 rayOrigin, float3 rayDirection, float3 v0, float3 v1, float3 v2)
{
    float3 e1 = v1 - v0;
    float3 e2 = v2 - v0;
    float3 s = rayOrigin - v0;
    float3 d = -rayDirection;
    float det = determinant(float3x3(d, e1, e2));
    if (det > 0.001f)
    {
        det = 1 / det;
        float u = det * determinant(float3x3(d, s, e2));
        float v = det * determinant(float3x3(d, e1, s));

        if ((u >= -0.001f && u <= 1.001f) && (v >= -0.001f && v <= (1.001f - u)))
        {
            return det * determinant(float3x3(s, e1, e2));
        }
    }
    return -1.f;
}


Hit RayTriangleIntersection(float3 rayOrig, float3 rayDir, Triangle tri, Hit previousHit)
{
    float3 p1 = tri.p1.xyz;
    float3 edge1 = tri.p2.xyz - p1;
    float3 edge2 = tri.p3.xyz - p1;
    
    float3 h = cross(rayDir, edge2);
    float a = dot(edge1, h);
    
    if (a * a < 0.00000000001f)
        return previousHit;

    float f = 1.0f / a;
    float3 s = rayOrig - p1;
    float u = f * dot(s, h);

    if (u < 0.0f)
        return previousHit;

    float3 q = cross(s, edge1);
    float v = f * dot(rayDir, q);

    if (v < 0.0f || u + v > 1.0f)
        return previousHit;

    float t = f * dot(edge2, q);
    if (t < 0.00001f)
    {
        float3 newHitPoint = rayOrig + rayDir * t;

        if (length(newHitPoint - rayOrig) < length(previousHit.hitPoint - rayOrig))
        {
            previousHit.hitPoint = newHitPoint;
            previousHit.normal = normalize(cross(edge1, edge2));
            previousHit.color = tri.color;
            previousHit.color = float4(previousHit.normal, 1);
        }
    }

    return previousHit;
}

float4 AccumulateLights(Hit hit)
{
    float4 c = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < iNumberOfLights; ++i)
        c += saturate(dot(hit.normal, normalize(mul(lights[i].position, vpMatrix).xyz - hit.hitPoint))) * lights[i].color;
    return c;
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 camPosition = float3(-4, -4, 6);
    Hit hit;
    hit.color = float4(0.3f, 0.0f, 0.0f, 1.0f);
    hit.hitPoint = float3(0, 0, 1000000);

    float3 dxdy = float3(DTid.x / (float) iScreenWidth * 2 - 1, -(DTid.y / (float) iScreenHeight * 2 - 1), 0.0f);
    float3 rayOrig = camPosition + dxdy;
    float3 focalPoint = camPosition - float3(0, 0, 0.5f);
    float3 rayDir = normalize(rayOrig - focalPoint);
    hit.color = float4(rayDir, 1);

    for (int i = 0; i < iNumberOfTriangles; ++i)
    {
        Triangle tri = triangles[i];

        hit = RayTriangleIntersection(rayOrig, rayDir, tri, hit); //RayVsTriangle(rayOrig, rayDir, tri.p1.xyz, tri.p2.xyz, tri.p3.xyz) != - 1 ? tri.color : hit.color; //RayTriangleIntersection(rayDir, rayOrig, tri, hit);

    }

    
    target[DTid.xy] = hit.color;
//    +AccumulateLights(hit);
  
}