/////////////
// GLOBALS //
/////////////

cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};

//////////////
// TYPEDEFS //
//////////////

struct HullInputType
{
	float3 position : POSITION;
	float4 color : COLOR; 
};

struct ConstantOutputType
{
	float edges[3] : SV_TessFactor; 
	float inside : SV_InsideTessFactor; 
};

struct HullOutputType
{
	float3 position : POSITION;
	float4 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output; 

	// Set the tessellation factors for 3 edges of the triangle.
	output.edges[0] = tessellationAmount;
	output.edges[1] = tessellationAmount;
	output.edges[2] = tessellationAmount;

	// Set the tessellation factor for tessellating inside the triangle.
	output.inside = tessellationAmount;

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOutputType output;

	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;

	// Set the input color as the output color.
	output.color = patch[pointId].color;

	return output; 
}