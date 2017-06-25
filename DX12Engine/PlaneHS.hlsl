Texture2D heightMap : register(t1);
SamplerState heightSamp : register(s1);


cbuffer MatrixBuffer : register(b0)
{
    float4x4 wvpMat;
    float padding[48];
};


cbuffer CameraPositionBuffer : register(b1)
{
    float3 cameraPostion;
};


// Input control point
struct VS_OUTPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : POSITION;
	float2 uv : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	
    float3 c = ip[0].position + ip[1].position + ip[2].position;
    c /= 3.0f;
    float2 uvs = ip[0].uv + ip[1].uv + ip[2].uv;
    uvs /= 300.0f;
    float y = heightMap.SampleLevel(heightSamp, uvs, 0).r;

    c.y += y * 100;
   
    float d = length(cameraPostion - c.xyz);

    //falloff function
    float f = min(75.0f/d, 10.0f);


	// Insert code to compute Output here
    Output.EdgeTessFactor[0] =
        Output.EdgeTessFactor[1] =
	    Output.EdgeTessFactor[2] = 
	    Output.InsideTessFactor = f;	// e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
	Output.vPosition = ip[i].position;
	Output.uv = ip[i].uv;

	return Output;
}
