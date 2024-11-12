/////////////
// GLOBALS //
/////////////

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix; 
};

cbuffer LightPositionBuffer
{
	float3 lightPosition; 
	float padding; 
};

//////////////
// TYPEDEFS //
//////////////

struct VertexInputType
{
	float4 position : POSITION; 
	float2 tex : TEXCOORD0; 
	float3 normal : NORMAL; 
};

struct PixelInputType
{
	float4 position : SV_POSITION; 
	float2 tex : TEXCOORD0; 
	float3 normal : NORMAL; 
	float4 viewPosition : TEXCOORD1; 
	float3 lightPos : TEXCOORD2; 
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SoftShadowVertexShader(VertexInputType input)
{
	PixelInputType output; 
	float4 worldPosition; 

	 // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the position of the vertices as viewed by the camera in a separate variable.
	output.viewPosition = output.position; 

	// Store the texture coordinates for the pixel shader.
	output.normal = mul(input.normal, (float3x3)worldMatrix); 

	// Normalize the normal vector.
	output.normal = normalize(output.normal); 

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz; 

	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos); 

	return output; 
}