////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Light = 0; 
	m_LightShader = 0; 
	m_FontShader = 0; 
	m_Font = 0; 
	m_TextString = 0; 
	m_MouseBitmap = 0; 
	m_TextureShader = 0; 
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename[128];
	char testString[32]; 
	bool result;

	// Store the screen width and height.
	m_screenWidth = screenWidth; 
	m_screenHeight = screenHeight; 

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.f, 0.f, -10.f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix(); 

	// Create and initialize the cube model object.
	m_Model = new ModelClass;

	strcpy_s(modelFilename, "../DirectXEngine/data/sphere.txt");
	strcpy_s(textureFilename, "../DirectXEngine/data/blue.tga");

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetDirection(0.f, 0.f, 1.f); 
	m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f); 

	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the font shader object.
	m_FontShader = new FontShaderClass;

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the text string object.
	m_TextString = new TextClass; 

	strcpy_s(testString, "Intersection: No");

	result = m_TextString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 
		screenWidth, screenHeight, 32, m_Font, testString, 10, 10, 0.0f, 1.0f, 0.0f);
	if (!result)
	{
		return false;
	}

	// Create and initialize the mouse bitmap object.
	m_MouseBitmap = new BitmapClass;

	strcpy_s(textureFilename, "../DirectXEngine/data/mouse.tga");

	result = m_MouseBitmap->Initilaize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight, textureFilename, 50, 50); 
	if (!result)
	{
		return false;
	}

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the mouse bitmap object.
	if (m_MouseBitmap)
	{
		m_MouseBitmap->Shutdown();
		delete m_MouseBitmap;
		m_MouseBitmap = 0;
	}

	// Release the text string object.
	if (m_TextString)
	{
		m_TextString->Shutdown();
		delete m_TextString;
		m_TextString = 0;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the cube model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	char testString[32]; 
	int mouseX, mouseY; 
	bool result; 
	bool intersect; 

	// Check if the user pressed escape and wants to exit the application.
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// Get the location of the mouse from the input object.
	Input->GetMouseLocation(mouseX, mouseY); 

	// Update the location of the mouse cursor on the Screen.
	m_MouseBitmap->SetRenderLocation(mouseX, mouseY); 

	// Check if the mouse intersects the sphere.
	intersect = TestIntersection(mouseX, mouseY); 

	// If it intersects, then update the text string message.
	if (intersect)
	{
		strcpy_s(testString, "Intersection : Yes"); 
	}
	else
	{
		strcpy_s(testString, "Intersection : No"); 
	}

	// Update the text string. 
	result = m_TextString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, testString, 10, 10, 0.f, 1.f, 0.f); 
	if (!result)
	{
		return false;
	}

	// Render the final graphics scene.
	result = Render(); 
	if (!result)
	{
		return false;
	}

	return true; 
}

bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX baseViewMatrix, orthoMatrix, translateMatrix; 
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.f, 0.5f, 0.8f, 1.f); 

	// Get the matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Translate to the location of the sphere.
	translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);

	// Render the full screen window using the glow shader.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), 
		translateMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Disable the Z buffer and enable alpha blending for 2D blending.
	m_Direct3D->TurnZBufferOff(); 
	m_Direct3D->EnableAlphaBlending(); 

	// Render the text string using the font shader.
	m_TextString->Render(m_Direct3D->GetDeviceContext()); 

	result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TextString->GetIndexCount(),
		worldMatrix, viewMatrix, baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(), m_TextString->GetPixelColor()); 
	if (!result)
	{
		retrun false;
	}

	// Render the mouse cursor using the texture shader.
	result = m_MouseBitmap->Render(m_Direct3D->GetDeviceContext()); 
	if (!result)
	{
		return false; 
	}

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_MouseBitmap->GetIndexCount(),
		worldMatrix, baseViewMatrix, orthoMatrix,
		m_MouseBitmap->GetTexture()); 
	if (!result)
	{
		return false;
	}

	// Enable the Z buffer and disable alpha blending now that 2D rendering is complete.
	m_Direct3D->TurnZBufferOn(); 
	m_Direct3D->DisableAlphaBlending(); 

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene(); 

	return true; 
}

bool ApplicationClass::TestIntersection(int mouseX, int mouseY)
{
	XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, inverseWorldMatrix; 
	XMFLOAT4X4 pMatrix, iViewMatrix; 
	XMVECTOR direction, origin; 
	XMVECTOR rayDirection, rayOrigin;
	XMFLOAT3 cameraDirection, cameraOrigin; 
	XMFLOAT3 rayOri, rayDir;
	float pointX, pointY; 
	bool intersect; 

	// Move the mouse cursor coordinates into the -1 to +1 range.
	pointX = ((2.f * (float)mouseX) / (float)m_screenWidth) - 1.f; 
	pointY = (((2.f * (float)mouseY) / (float)m_screenHeight) - 1.f) * -1.f;

	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	m_Direct3D->GetProjectionMatrix(projectionMatrix); 
	XMStoreFloat4x4(&pMatrix, projectionMatrix); 
	pointX /= pMatrix._11; 
	pointY /= pMatrix._22; 

	// Get the inverse of the view matrix.
	m_Camera->GetViewMatrix(viewMatrix); 
	inverseViewMatrix = XMMatrixInverse(NULL, viewMatrix); 
	XMStoreFloat4x4(&iViewMatrix, inverseViewMatrix); 

	// Calculate the direction of the picking ray in view space.
	cameraDirection.x = (pointX * iViewMatrix._11) + (pointY * iViewMatrix._21) + iViewMatrix._31; 
	cameraDirection.y = (pointX * iViewMatrix._12) + (pointY * iViewMatrix._22) + iViewMatrix._32; 
	cameraDirection.z = (pointX * iViewMatrix._13) + (pointY * iViewMatrix._32) + iViewMatrix._33; 
	direction = XMLoadFloat3(&cameraDirection); 

	// Get the origin of the picking ray which is the position of the camera.
	cameraOrigin = m_Camera->GetPosition();
	origin = XMLoadFloat3(&cameraOrigin); 

	// Get the world matrix and translate to the location of the sphere.
	worldMatrix = XMMatrixTranslation(-5.f, 1.f, 5.f); 

	// Now get the inverse of the translated world matrix.
	inverseWorldMatrix = XMMatrixInverse(NULL, worldMatrix); 

	// Now transform the ray origin and the ray direction from view space to world space.
	rayOrigin = XMVector3TransformCoord(origin, inverseWorldMatrix); 
	rayDirection = XMVector3TransformNormal(direction, inverseWorldMatrix); 

	// Normalize the ray direction. 
	rayDirection = XMVector3Normalize(rayDirection); 

	// Convert the ray origin and direction XMVECTOR to a XMFLOAT3 type.
	XMStoreFloat3(&rayOri, rayOrigin);
	XMStoreFloat3(&rayDir, rayDirection); 

	// Now perform the ray-sphere intersection test.
	intersect = RaySphereIntersect(rayOri, rayDir, 1.f); 

	return intersect; 
}

bool ApplicationClass::RaySphereIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, float radius)
{
	float a, b, c, discriminant;

	// Calculate the a, b, and c coefficients.
	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z); 
	b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.f; 
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius); 

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c); 

	// If the discriminant is negative, the picking ray missed sphere, otherwise, it intersected the sphere.
	if (discriminant < 0.f)
	{
		return false; 
	}

	return true; 
}




