////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_CubeModel = 0;
	m_FloorModel = 0;
	m_RenderTexture = 0;
	m_TextureShader = 0;
	m_ReflectionShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename[128;
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();

	// Set the file name of the model.
	strcpy_s(modelFilename, "../DirectXEngine/data/cube.txt");

	// Set the file names of the textures.
	strcpy_s(textureFilename, "../DirectXEngine/data/stone01.tga");

	// Create and initialize the cube model object.
	m_CubeModel = new ModelClass;

	result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}


	// Set the file name of the floor model.
	strcpy_s(modelFilename, "../DirectXEngine/data/floor.txt");

	// Set the file name of the texture.
	strcpy_s(textureFilename, "../DirectXEngine/data/blue01.tga");

	// Create and initialize the floor model object.
	m_FloorModel = new ModelClass;

	result = m_FloorModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
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

	// Create and initialize the transparent shader object.
	m_TransparentShader = new TransparentShaderClass;

	result = m_TransparentShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the transparent shader object.
	if (m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	
	// Release the model object.
	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	// Release the model object.
	if (m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
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
	static float textureTranslation = 0.f; 
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if(Input->IsEscapePressed())
	{
		return false;
	}

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::RenderReflectionToTexture(float)
{
	return false;
}


bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float blendAmount; 
	bool result;

	// Set the blending amount to 50%. 
	blendAmount = 0.5f; 

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix); 
	m_Camera->GetViewMatrix(viewMatrix); 
	m_Direct3D->GetProjectionMatrix(projectionMatrix); 

	// Render the first model that is using the dirt texture using the regular texture shader.
	m_Model1->Render(m_Direct3D->GetDeviceContext());

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model1->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, m_Model1->GetTexture());
	if (!result)
	{
		return false;
	}
	
	// Translate to the right by 1 unit and towards the camera by 1 unit. 
	worldMatrix = XMMatrixTranslation(1.f, 0.f, -1.f); 

	// Turn on alpha blending for the transparency to work. 
	m_Direct3D->EnableAlphaBlending(); 

	// Render the second square model with the stone texture and use the 50% blending amount for transparency.
	m_Model2->Render(m_Direct3D->GetDeviceContext());

	result = m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Model2->GetTexture(), blendAmount);
	if (!result) { return false; }

	// Turn off alpha blending. 
	m_Direct3D->DisableAlphaBlending(); 

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
