#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0; 
	m_hullShader = 0; 
	m_domainShader = 0; 
	m_pixelShader = 0; 
	m_layout = 0; 
	m_matrixBuffer = 0; 
	m_tessellationBuffer = 0; 
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128]; 
	wchar_t hsFilename[128]; 
	wchar_t dsFilename[128]; 
	wchar_t psFilename[128]; 
	int error; 
	bool result; 

	// Set the filename of the vertex shader. 
	error = wcscpy_s(vsFilename, 128, L"C:\DirectXProject\DirectX_Learning\DirectXEngine\color.vs");
	if (error != 0) { return false; }

	// Set the filename of the hull shader.
	error = wcscpy_s(hsFilename, 128, L"../DirectXEngine/color.hs");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the domain shader.
	error = wcscpy_s(dsFilename, 128, L"../DirectXEngine/color.ds");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the pixel shader. 
	error = wcscpy_s(psFilename, 128, L"C:\DirectXProject\DirectX_Learning\DirectXEngine\color.ps"); 
	if (error != 0) { return false; }

	// Initialize the vertex and pixel shaders. 
	result = InitializeShader(device, hwnd, vsFilename, hsFilename, dsFilename, psFilename); 
	if (!result) { return false; }

	return true; 
}

void ColorShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects. 
	ShutdownShader(); 

	return; 
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	float tessellationAmount)
{
	bool result; 

	// Set the shader parameters that it will use for rendering. 
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount); 
	if (!result) { return false; }

	// Now render the prepared buffers with the shader. 
	RenderShader(deviceContext, indexCount); 

	return true; 
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	HRESULT result; 
	ID3D10Blob* errorMessage; 
	ID3D10Blob* vertexShaderBuffer; 
	ID3D10Blob* hullShaderBuffer; 
	ID3D10Blob* domainShaderBuffer; 
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; 
	unsigned int numElements; 
	D3D11_BUFFER_DESC matrixBufferDesc; 
	D3D11_BUFFER_DESC tessellationBufferDesc; 

	// Initialize the pointers this function will use to  null. 
	errorMessage = 0; 
	vertexShaderBuffer = 0; 
	hullShaderBuffer = 0; 
	domainShaderBuffer = 0; 
	pixelShaderBuffer = 0; 

	// Compile the vertex shader code. 
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage); 
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

	// Compile the hull shader code.
	result = D3DCompileFromFile(hsFilename, NULL, NULL, "ColorHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&hullShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the domain shader code.
	result = D3DCompileFromFile(dsFilename, NULL, NULL, "ColorDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&domainShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage); 
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

	// Create the hull shader from the buffer.
	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the domain shader from the buffer.
	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer. 
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader); 
	if (FAILED(result)) { return false; }

	// Create the vertex input layout description. 
	// This setup needs to match the VertexType structure in ModelClass and in the shader. 
	polygonLayout[0].SemanticName = "POSITION"; 
	polygonLayout[0].SemanticIndex = 0; 
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
	polygonLayout[0].InputSlot = 0; 
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
	polygonLayout[0].InstanceDataStepRate = 0; 

	polygonLayout[1].SemanticName = "COLOR"; 
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
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

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer); 
	if (FAILED(result)) { return false; }

	// Setup the description of the dynamic tessellation constant buffer that is in the hull shader.
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the hull shader constant buffer from within this class.
	result = device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// Release the tessellation constant buffer.
	if (m_tessellationBuffer)
	{
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
	}

	// Release the matrix constant buffer. 
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release(); 
		m_matrixBuffer = 0; 
	}

	// Release the layout. 
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0; 
	}

	// Release the pixel shader. 
	if (m_pixelShader)
	{
		m_pixelShader->Release(); 
		m_pixelShader = 0; 
	}

	// Release the domain shader.
	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// Release the hull shader.
	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0; 
	}

	return; 
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors; 
	unsigned long long bufferSize, i; 
	ofstream fout; 

	// Get a pointer to the error message text buffer. 
	compileErrors = (char*)(errorMessage->GetBufferPointer()); 

	// Get the length of the message. 
	bufferSize = errorMessage->GetBufferSize(); 

	// Open a file to write the error message to 
	fout.open("shader-error.txt"); 

	// Write out the error message
	for (i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i]; 
	}

	// Close the file. 
	fout.close(); 

	// Release the error message 
	errorMessage->Release(); 
	errorMessage = 0; 

	// Pop a message up on the screen to notify the user to check the text file for compile errors. 
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK); 

	return; 
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	float tessellationAmount)
{
	HRESULT result; 
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	MatrixBufferType* dataPtr; 
	unsigned int bufferNumber; 
	TessellationBufferType* dataPtr2; 

	// Transpose the matrices to prepare them for the shader. 
	worldMatrix = XMMatrixTranspose(worldMatrix); 
	viewMatrix = XMMatrixTranspose(viewMatrix); 
	projectionMatrix = XMMatrixTranspose(projectionMatrix); 

	// Lock the constant buffer, so it can be written to. 
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the constant buffer. 
	dataPtr = (MatrixBufferType*)mappedResource.pData; 

	// Copy the matrices into the constant buffer. 
	dataPtr->world = worldMatrix; 
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix; 

	// Unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0); 

	// Set the position of the constant buffer in the vertex shader. 
	bufferNumber = 0; 

	// Finally set the matrix constant buffer in the domain shader with the updated values.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer); 

	// Lock the tessellation constant bufffer so it can be written to.
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the tessellation constant buffer.
	dataPtr2 = (TessellationBufferType*)mappedResource.pData; 

	// Copy the tessellation data into the constant buffer.
	dataPtr2->tessellationAmount = tessellationAmount;
	dataPtr2->padding = XMFLOAT3(0.f, 0.f, 0.f); 

	// Unlock the tessellation constant buffer.
	deviceContext->Unmap(m_tessellationBuffer, 0); 

	// Set the position of the tessellation constant buffer in the hull shader.
	bufferNumber = 0; 

	// Now set the tessellation consatnt buffer in the hull shader with the updated values.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer); 

	return true; 
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout. 
	deviceContext->IASetInputLayout(m_layout); 

	// Set the vertex and pixel shaders that will be used to render this triangle. 
	deviceContext->VSSetShader(m_vertexShader, NULL, 0); 
	deviceContext->HSSetShader(m_hullShader, NULL, 0); 
	deviceContext->DSSetShader(m_domainShader, NULL, 0); 
	deviceContext->PSSetShader(m_pixelShader, NULL, 0); 

	// Render the triangle. 
	deviceContext->DrawIndexed(indexCount, 0, 0); 

	return; 
}

