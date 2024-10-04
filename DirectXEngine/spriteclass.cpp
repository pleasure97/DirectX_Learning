#include "spriteclass.h"

SpriteClass::SpriteClass()
{
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_Textures = 0; 
}

SpriteClass::SpriteClass(const SpriteClass&)
{
}

SpriteClass::~SpriteClass()
{
}

bool SpriteClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	int screenWidth, int screenHeight, char* spriteFilename, int renderX, int renderY)
{
	bool result; 

	// Store the screen size. 
	m_screenWidth = screenWidth; 
	m_screenHeight = screenHeight; 

	// Store where the sprite should be rendered to. 
	m_renderX = renderX; 
	m_renderY = renderY;

	// Initialize the frame time for the sprite object. 
	m_frameTime = 0; 

	// Initialize the vertex and index buffer that hold the geometry for the sprite bitmap.
	result = InitializeBuffers(device); 
	if (!result) { return false; }

	// Load the textures for this sprite. 
	result = LoadTextures(device, deviceContext, spriteFilename); 
	if (!result) { return false; }

	return true; 
}

void SpriteClass::Shutdown()
{
	// Release the textures used for this sprite. 
	ReleaseTextures(); 

	// Release the vertex and index buffers. 
	ShutdownBuffers(); 

	return;
}

bool SpriteClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result; 

	// Update the buffers if the position of the sprite has changed from its original position.
	result = UpdateBuffers(deviceContext); 
	if (!result) { return false; }

	// Put the vertex and index buffers on the graphcis pipeline to prepare them for drawing. 
	RenderBuffers(deviceContext); 

	return true; 
}

void SpriteClass::Update(float frameTime)
{
	// Increment the frame time each frame. 
	m_frameTime += frameTime; 

	// Check if the frame has reached the cycle time
	if (m_frameTime > m_cycleTime)
	{
		// If it has, then reset the frame time and cycle to the next sprite in the texture array.
		m_frameTime -= m_cycleTime; 

		m_currentTexture++;

		// If we are at the last sprite texture, then go back to the beginning of the texture array to the first texture again. 
		if (m_currentTexture == m_textureCount)
		{
			m_currentTexture = 0; 
		}
	}

	return; 
}

int SpriteClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* SpriteClass::GetTexture()
{
	return m_Textures[m_currentTexture].GetTexture(); 
}

void SpriteClass::SetRenderLocation(int x, int y)
{
	m_renderX = x; 
	m_renderY = y; 

	return;
}

bool SpriteClass::InitializeBuffers(ID3D11Device* device)
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

	// Set the number of vertices in vertex array. 
	m_vertexCount = 6; 

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array. 
	vertices = new VertexType[m_vertexCount]; 

	// Create the index array. 
	indices = new unsigned long[m_indexCount]; 

	// Initialize vertex array to 0s at first. 
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount)); 

	// Load the index array with the data. 
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

	// Give the subresource structure a pointer to the index data. 
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
	indices = 0; 

	return true; 
}

void SpriteClass::ShutdownBuffers()
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

bool SpriteClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* dataPtr;
	HRESULT result;

	// If the position we are renderin this bitmap to hasn't changed, then don't update the vertex buffer. 
	if ((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY))
	{
		return true;
	}

	// If the rendering location has changed, then store the new position and update the vertex buffer. 
	m_prevPosX = m_renderX;
	m_prevPosY = m_renderY;

	// Create the vertex array. 
	vertices = new VertexType[m_vertexCount];

	// Calculate the screen coordinates of the left side of the bitmap. 
	left = (float)((m_screenWidth / 2) * -1) + (float)m_renderX;
	right = left + (float)m_bitmapWidth;
	top = (float)(m_screenHeight / 2) - (float)m_renderY;
	bottom = top - (float)m_bitmapHeight;

	// Load the vertex array with data. 
	// First triangle. 
	vertices[0].position = XMFLOAT3(left, top, 0.f);
	vertices[0].texture = XMFLOAT2(0.f, 0.f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.f);
	vertices[1].texture = XMFLOAT2(1.f, 1.f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.f);
	vertices[2].texture = XMFLOAT2(0.f, 1.f);

	// Second triangle
	vertices[3].position = XMFLOAT3(left, top, 0.f);
	vertices[3].texture = XMFLOAT2(0.f, 0.f);

	vertices[3].position = XMFLOAT3(right, top, 0.f);
	vertices[3].texture = XMFLOAT2(1.f, 0.f);

	vertices[3].position = XMFLOAT3(right, bottom, 0.f);
	vertices[3].texture = XMFLOAT2(1.f, 1.f);

	// Lock the vertex buffer. 
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the constant buffer.
	dataPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer. 
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer. 
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the pointer reference. 
	dataPtr = 0;

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride; 
	unsigned int offset; 

	// Set vertex buffer stride and offset. 
	stride = sizeof(VertexType); 
	offset = 0; 

	// Set the vertex buffer to be active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset); 

	// Set the index buffer to be active in the input assembler so it can be rendered. 
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0); 

	// Set the type of primitive that should be rendered from this vertex buffer, in this case, triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
}

bool SpriteClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	char textureFilename[128]; 
	ifstream fin; 
	int i, j;
	char input; 
	bool result; 

	// Open the sprite info data file.
	fin.open(filename); 
	if (fin.fail()) { return false; }

	// Read in the number of textures. 
	fin >> m_textureCount; 

	// Create and initialize the texture array with the texture count from the file.
	m_Textures = new TextureClass[m_textureCount]; 

	// Read to start of next line.
	fin.get(input); 

	// Read in each texture file name.
	for (i = 0; i < m_textureCount; ++i)
	{
		j = 0; 
		fin.get(input); 
		while (input != '\n')
		{
			textureFilename[j] = input; 
			j++; 
			fin.get(input); 
		}
		textureFilename[j] = '\0';

		// Once you have the filename, then load the texture in the texture array. 
		result = m_Textures[i].Initialize(device, deviceContext, textureFilename); 
		if (!result) { return false; }
	}

	// Read in the cycle time. 
	fin >> m_cycleTime; 

	// Convert the integer miliseconds to float representation. 
	m_cycleTime = m_cycleTime * 0.001f; 

	// close the file.
	fin.close(); 

	// Get the dimension of the first texture and use that as the dimensions of 2D sprite images. 
	m_bitmapWidth = m_Textures[0].GetWidth();
	m_bitmapHeight = m_Textures[0].GetHeight(); 

	// Set the starting texture in the cycle to be the first one in the list. 
	m_currentTexture = 0; 

	return true; 
}

void SpriteClass::ReleaseTextures()
{
	int i; 

	// Release the texture objects. 
	if (m_Textures)
	{
		for (i = 0; i < m_textureCount; ++i)
		{
			m_Textures[i].Shutdown();
		}

		delete[] m_Textures; 
		m_Textures = 0; 
	}

	return; 
}
