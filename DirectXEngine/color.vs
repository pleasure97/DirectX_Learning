// GLOBALS // 

cbuffer MatrixBuffer
{
	matrix worldMatrix; 
	matrix viewMatrix; 
	matrix projectionMatrix; 
};

// TYPEDEFS // 
struct VertexInputType
{
	float4 position : POSITION; 
	float4 color : COLOR; 
};

struct HullInputType
{
	float3 position : POSITION; 
	float4 color : COLOR; 
};

// VERTEX SHADER // 

HullInputType ColorVertexShader(VertexInputType input)
{
	HullInputType output; 

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	// Store the input color into the hull shader.
	output.color = input.color; 

	return output; 
};