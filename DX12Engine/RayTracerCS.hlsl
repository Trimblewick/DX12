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
    float pad1;
    float4 color;
    float3 normal;
    float pad2;
};

struct Ray
{
    float3 origin;
    float bounce;
    float3 direction;
    float pad;
};


bool RayTriangleIntersection(inout Ray ray, in Triangle tri, inout Hit hit)
{
    float3 p1 = tri.p1.xyz;
    float3 edge1 = tri.p2.xyz - p1;
    float3 edge2 = tri.p3.xyz - p1;
    
    float3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);
    
    if (a * a < 0.00000000001f)
        return false;

    float f = 1.0f / a;
    float3 s = ray.origin - p1;
    float u = f * dot(s, h);

    if (u < 0.0f)
        return false;

    float3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = f * dot(edge2, q);
    if (t < 0.00001f)
    {
        float3 newHitPoint = ray.origin + ray.direction * t;

        if (length(newHitPoint - ray.origin) < length(hit.hitPoint - ray.origin))
        {
            hit.hitPoint = newHitPoint;
            hit.normal = normalize(cross(edge1, edge2));
            hit.color = tri.color;

            ray.origin = newHitPoint;
            ray.direction = newHitPoint + hit.normal;
            //previousHit.color = float4(previousHit.normal, 1);
        }
        return true;
    }

    return false;
}

float4 AccumulateLights(Hit hit, float3 camPosition)
{
    float4 c = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //int i = 0;
    

    float magnitude = 1.0f;

    c += saturate(dot(hit.normal, normalize(lights[0].position - camPosition))) * (3.0f / magnitude) * lights[0].color;
        
    return c;
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 camPosition = camPositionpp;
//    float3(-4, -4, 6);
    Hit hit;
    hit.pad1 = 0;
    hit.pad2 = 0;
    hit.color = float4(0.3f, 0.0f, 0.0f, 1.0f);
    hit.hitPoint = float3(0, 0, 1000000);

    Ray ray;
    ray.bounce = 0;
    ray.pad = 0;
    float3 dxdy = float3(DTid.x / (float) iScreenWidth * 2 - 1, -(DTid.y / (float) iScreenHeight * 2 - 1), 0.0f);
    ray.origin = camPosition + dxdy;
    float3 focalPoint = camPosition - float3(0, 0, 0.5f);
    ray.direction = normalize(ray.origin - focalPoint);
    hit.color = float4(0, 0.3f, 0, 1);



    for (int i = 0; i < iNumberOfTriangles; ++i)
    {
        Triangle tri = triangles[i];

        RayTriangleIntersection(ray, tri, hit); 
    }

    Ray lightRay;
    lightRay.bounce = 0;
    lightRay.pad = 0;
    Hit lightHit;
    lightHit.pad1 = 0;
    lightHit.pad2 = 0;
    
    
    for (int l = 0; l < iNumberOfLights; ++l)
    {
        lightRay.origin = ray.origin;
        float3 lightPos = lights[l].position.xyz;
        lightRay.direction = normalize(lightPos - ray.origin);
        lightHit.hitPoint = lightPos;
        
        bool bInLight = true;
        for (int k = 0; k < iNumberOfTriangles; ++k)
        {
            Triangle tri = triangles[k];

            RayTriangleIntersection(lightRay, tri, lightHit);
            if (length(lightHit.hitPoint - ray.origin) < length(lightPos - ray.origin))
            {
            
                k = iNumberOfTriangles;
                bInLight = false;
            }
            
        }
        if (bInLight)
        {
            //hit.color += float4(0.05f, 0.05f, 0.05f, 0.0f);
        }
    }
        
    
    target[DTid.xy] = hit.color +AccumulateLights(hit, camPosition);
  
}