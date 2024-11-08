////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z); 
	return; 
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = XMFLOAT3(x, y, z); 
	return; 
}


XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}

XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::GenerateViewMatrix()
{
	XMFLOAT3 up; 
	XMVECTOR positionVector; 
	XMVECTOR lookAtVector; 
	XMVECTOR upVector; 

	// Setup the vector that points upwards.
	up.x = 0.f; 
	up.y = 1.f;
	up.z = 0.f; 

	// Load the XMFLOAT3 into XMVECTOR
	positionVector = XMLoadFloat3(&m_position); 
	lookAtVector = XMLoadFloat3(&m_lookAt); 
	upVector = XMLoadFloat3(&up); 

	// Create the view matrix from the 3 vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector); 

	return; 
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView; 
	float screenAspect; 

	// Setup field of view and screen aspect of a square light source.
	fieldOfView = 3.14159265358979323846f / 2.f;
	screenAspect = 1.f;

	// Create the projection matrix for the light.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth); 

	return;
}

void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix; 
	return; 
}

void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix; 
	return; 
}

void LightClass::GenerateOrthoMatrix(float width, float nearPlane, float depthPlane)
{
	// Create the orthographic matrix for the light.
	m_orthoMatrix = XMMatrixOrthographicLH(width, width, nearPlane, depthPlane); 
	return; 
}

void LightClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix; 
	return; 
}
