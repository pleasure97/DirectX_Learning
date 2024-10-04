#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_Texture = 0; 
}

BitmapClass::BitmapClass(const BitmapClass&)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initilaize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	int screenWidth, int screenHeight, char* textureFilename, int renderX, int renderY)
{
	bool result; 

	// Store the screen size. 
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight; 

	// Store when the bitmap should be rendered to. 
	m_renderX = renderX; 
	m_renderY = renderY; 

	// Initialize the vertex and index bufer that hold the geometry for the bitmap quad. 
	result = InitializeBuffers(device); 
	if (!result) { return false;  }

	// Load the texture for the bitmap.  
	result = LoadTexture(device, deviceContext, textureFilename); 
	if (!result) { return false; }

	return true; 
}

void BitmapClass::Shutdown()
{
	// Release the bitmap texture. 
	ReleaseTexture(); 

	// Release the vertex and index buffers. 
	ShutdownBuffers(); 

	return; 
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result; 

	// Update the buffers if the position of the bitmap has changed from its original position. 
	result = UpdateBuffers(deviceContext); 
	if (!result) { return false; }

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing. 
	RenderBuffers(deviceContext); 

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void BitmapClass::SetRenderLocation(int x, int y)
{
	m_renderX = x; 
	m_renderY = y; 

	return; 
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices; 
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc; 
	D3D11_BUFFER_DESC indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData; 
	D3D11_SUBRESOURCE_DATA indexData; 
	HRESULT result;
	int i; 

	// Initialize the previous rendering position to -1.
	m_prevPosX = -1; 
	m_prevPosY = -1; 

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6; 

	// Set the number of indices in the index array. 
	m_indexCount = m_vertexCount; 

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount]; 

	// Create the index array. 
	indices = new unsigned long[m_indexCount]; 

	// Initialize vertex array to 0s at first. 
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount)); 

	// Load the index array with data. 
	for (i = 0; i < m_indexCount; ++i)
	{
		indices[i] = i; 
	}
	
	// Set up the description of the dynamic vertex buffer. 
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	vertexBufferDesc.MiscFlags = 0; 
	vertexBufferDesc.StructureByteStride = 0; 

	// Give the subresource structure a pointer to the vertex data. 
	vertexData.pSysMem = vertices; 
	vertexData.SysMemPitch = 0; 
	vertexData.SysMemSlicePitch = 0; 

	// Now finally create the vertex buffer. 
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); 
	if (FAILED(result)) { return false; }

	// Set up the description of the index buffer. 
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount; 
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0; 

	// Give the subresource struture a pointer to the index data. 
	indexData.pSysMem = indices; 
	indexData.SysMemPitch = 0; 
	indexData.SysMemSlicePitch = 0; 

	// Create the index buffer. 
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer); 
	if (FAILED(result)) { return false; }

	// Release the arrays now that the vertex and index buffers have been created and loaded. 
	delete[] vertices; 
	vertices = 0; 

	delete[] indices; 
	vertices = 0; 

	return true; 
}

void BitmapClass::ShutdownBuffers()
{
	// Release the index buffer. 
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0; 
	}

	// Release the vertex buffer. 
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release(); 
		m_vertexBuffer = 0; 
	}

	return; 
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	float left, right, top, bottom; 
	VertexType* vertices; 
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	VertexType* dataPtr; 
	HRESULT result; 

	// If the position we are rendering this bitmap to hasn't changed, then don't update the vertex buffer. 
	if ((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY)){ return true; }

	// If the rendering location has changd, then store the new position and update the vertex buffer. 
	m_prevPosX = m_renderX; 
	m_prevPosY = m_renderY; 

	// Craete the vertex array. 
	vertices = new VertexType[m_vertexCount]; 

	// Calculate the screen coordinates of the left side of the bitmap. 
	left = (float)((m_screenWidth / 2) * -1) + (float)m_renderX; 

	// Calculate the screen coordinates of the right side of the bitmap. 
	right = left + (float)m_bitmapWidth; 

	// Calculate the screen coordinates of the top of the bitmap. 
	top = (float)(m_screenHeight / 2) - (float)m_renderY; 

	// Calculate the screen coordinates of the bottom of the bitmap. 
	bottom = top - (float)m_bitmapHeight; 

	// Load the vertex array with data. 
	// First triangle.
	vertices[0].position = XMFLOAT3(left, top, 0.f); // Top left
	vertices[0].texture = XMFLOAT2(0.f, 0.f); 

	vertices[1].position = XMFLOAT3(right, bottom, 0.f); // Bottom right 
	vertices[1].texture = XMFLOAT2(1.f, 1.f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.f); // Bottom left 
	vertices[2].texture = XMFLOAT2(0.f, 1.f);

	// Second triangle. 
	vertices[3].position = XMFLOAT3(left, top, 0.f); // Top left
	vertices[3].texture = XMFLOAT2(0.f, 0.f); 

	vertices[4].position = XMFLOAT3(right, top, 0.f); // Top left
	vertices[4].texture = XMFLOAT2(1.f, 0.f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.f); // Top left
	vertices[5].texture = XMFLOAT2(1.f, 1.f);

	// Lock the vertex buffer. 
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the constant buffer. 
	dataPtr = (VertexType*)mappedResource.pData; 

	// Copy the data into the vertex buffer.
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer. 
	deviceContext->Unmap(m_vertexBuffer, 0); 

	// Release the pointer referecne. 
	dataPtr = 0; 

	// Release the vertex array as it's no longer needed. 
	delete[] vertices; 
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride; 
	unsigned int offset; 

	// Set the vertex buffer stride and offset. 
	stride = sizeof(VertexType); 
	offset = 0; 

	// Set the vertex buffer to be active in the input assembler so it can be rendered. 
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset); 

	// Set the index buffer to be active in the input assembler so it can be rendered. 
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0); 

	// Set the type of primitive that should be rendered from this vertex buffer, in this case, triangles. 
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 

	return; 
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result; 

	// Create and initialize the texture object. 
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename); 
	if (!result) { return false; }

	// Store the size in pixels that this bitmap should be rendered at. 
	m_bitmapWidth = m_Texture->GetWidth(); 
	m_bitmapHeight = m_Texture->GetHeight(); 

	return true; 
}

void BitmapClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown(); 
		delete m_Texture; 
		m_Texture = 0; 
	}

	return;
}
