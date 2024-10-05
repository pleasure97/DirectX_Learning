////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_MultiTextureShader = 0; 
	m_Model = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128]; 
	char textureFilename1[128]; 
	char textureFilename2[128]; 
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.f, -5.f);
	m_Camera->Render(); 

	// Create and initialize the multitexture shader object. 
	m_MultiTextureShader = new MultiTextureShaderClass;

	result = m_MultiTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK); 
		return false; 
	}

	// Set the file name of the model.
	strcpy_s(modelFilename, "../DirectXEngine/data/square.txt"); 

	// Set the file name of the textures. 
	strcpy_s(textureFilename1, "../DirectXEngine/data/ston01.tga");
	strcpy_s(textureFilename2, "../DirectXEngine/data/dirt01.tga");

	// Create and initialize the model object. 
	m_Model = new ModelClass; 

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2); 
	if (!result) { return false; }

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the model object. 
	if (m_Model)
	{
		m_Model->Shutdown(); 
		delete m_Model; 
		m_Model = 0;
	}

	// Release the multitexture shader object. 
	if (m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown(); 
		delete m_MultiTextureShader; 
		m_MultiTextureShader = 0; 
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	int mouseX, mouseY; 
	bool result; 
	bool mouseDown;

	// Check if the user processed escape and wants to exit the program. 
	if (Input->IsEscapePressed())
	{
		return false;
	}

	// Get the location of the mouse from the input object. 
	Input->GetMouseLocation(mouseX, mouseY); 

	// Check if the mouse has been pressed.
	mouseDown = Input->IsMousePressed(); 

	// Render the graphics scene. 
	result = Render(); 
	if (!result) { return false; }

	return true; 
}


bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix); 

	// Render the model using the multitexture shader. 
	m_Model->Render(m_Direct3D->GetDeviceContext()); 

	result = m_MultiTextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Model->GetTexture(1));
	if (!result) { return false; }

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


