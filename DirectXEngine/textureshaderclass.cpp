#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0; 
	m_pixelShader = 0; 
	m_layout = 0; 
	m_matrixBuffer = 0; 
	
	m_sampleState = 0; 
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass&)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result; 
	wchar_t vsFilename[128]; 
	wchar_t psFilename[128]; 
	int error; 

	// Set the filename of the vertex shader. 
	error = wcscpy_s(vsFilename, 128, L"../DirectXEngine/texture.vs"); 
	if (error != 0) { return false; }

	// Set the file name of the pixel shader. 
	error = wcscpy_s(psFilename, 128, L"../DirectXEngine/texture.ps"); 
	if (error != 0) { return false; }

	// Initialize the vertex and pixel shaders. 
	result = InitializeShader(device, hwnd, vsFilename, psFilename); 
	if (!result) { return false; }

	return true; 
}

void TextureShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects. 
	ShutdownShader(); 

	return; 
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result; 

	// Set the shader parameters that it will use for rendering. 
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture); 
	if (!result) { return false; }

	// Now render the prepared buffers with the shader. 
	RenderShader(deviceContext, indexCount); 
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result; 
	ID3D10Blob* errorMessage; 
	ID3D10Blob* vertexShaderBuffer; 
	ID3D10Blob* pixelShaderBuffer; 
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; 
	unsigned int numElements; 
	D3D11_BUFFER_DESC matrixBufferDesc; 

	D3D11_SAMPLER_DESC samplerDesc; 

	// Initialize the pointers thsi function will use to null. 
	errorMessage = 0; 
	vertexShaderBuffer = 0; 
	pixelShaderBuffer = 0; 

	// Compile the new texture vertex and pixel shaders. 
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile, it should have written something to the error message. 
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); 
		}
		// If there was nothing in the error message, then it simply could not find the shader file itself. 
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); 
		}

		return false; 
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage); 
	if (FAILED(result))
	{
		// If the shader failed to compile, it should have written something to the error message. 
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename); 
		}
		// If there was nothing in the error message, then it simply could not find the shader file itself. 
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK); 
		}

		return false; 
	}

	// Create the vertex shader from the buffer. 
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader); 
	if (FAILED(result)) { return false; }

	// Create the pixel shader from the buffer. 
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader); 
	if (FAILED(result)) { return false; }

	// Create the vertex input layout description. 
	// This setup needs to match the VertexType structure in the ModelClass and in the shader. 
	polygonLayout[0].SemanticName = "POSITION"; 
	polygonLayout[0].SemanticIndex = 0; 
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
	polygonLayout[0].InstanceDataStepRate = 0; 

	polygonLayout[1].SemanticName = "TEXCOORD"; 
	polygonLayout[1].SemanticIndex = 0; 
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT; 
	polygonLayout[1].InputSlot = 0; 
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; 
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
	polygonLayout[1].InstanceDataStepRate = 0; 

	// Get a count of the elements in the layout. 
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]); 

	// Create the vertex input layout. 
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout); 
	if (FAILED(result)) { return false; }

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed. 
	vertexShaderBuffer->Release(); 
	vertexShaderBuffer = 0; 

	pixelShaderBuffer->Release(); 
	pixelShaderBuffer = 0; 

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader. 
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	matrixBufferDesc.MiscFlags = 0; 
	matrixBufferDesc.StructureByteStride = 0; 

	// Create the constant buffer pointer so we can access the vertex shader constant buffer within this class. 
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }
}

void TextureShaderClass::ShutdownShader()
{
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*)
{
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*)
{
	return false;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext*, int)
{
}
