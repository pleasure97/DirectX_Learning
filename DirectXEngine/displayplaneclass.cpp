#include "displayplaneclass.h"

DisplayPlaneClass::DisplayPlaneClass()
{
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
}

DisplayPlaneClass::DisplayPlaneClass(const DisplayPlaneClass&)
{
}

DisplayPlaneClass::~DisplayPlaneClass()
{
}

bool DisplayPlaneClass::Initialize(ID3D11Device* device, float width, float height)
{
	bool result; 

	// Initialize the vertex and index buffer that hold the geomerty for the button. 
	result = InitializeBuffers(device, width, height); 
	if (!result) { return false; }

	return true;
}

void DisplayPlaneClass::Shutdown()
{
	// Release the vertex and index buffers. 
	ShutdownBuffers(); 

	return; 
}

void DisplayPlaneClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphcis pipelint to prepare them for drawing. 
	RenderBuffers(deviceContext); 

	return; 
}

int DisplayPlaneClass::GetIndexCount()
{
	return m_indexCount; 
}

bool DisplayPlaneClass::InitializeBuffers(ID3D11Device* device, float width, float height)
{
	VertexType* vertices; 
	unsigned long* indices; 
	D3D11_BUFFER_DESC vertexBufferDesc; 
	D3D11_BUFFER_DESC indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData; 
	D3D11_SUBRESOURCE_DATA indexData; 
	HRESULT result; 
	int i; 

	// Set the number of vertices in the vertex array. 
	m_vertexCount = 6; 

	// Set the number of indices in the index array. 
	m_indexCount = m_vertexCount; 

	// Create the vertex array. 
	vertices = new VertexType[m_vertexCount]; 

	// Create the index array. 
	indices = new unsigned long[m_indexCount]; 

	// Load the vertex array with data. 
	// First triangle. 
	vertices[0].position = XMFLOAT3(-width, height, 0.f); // Top Left
	vertices[0].texture = XMFLOAT2(0.f, 0.f); 

	vertices[1].position = XMFLOAT3(width, -height, 0.f);
	vertices[1].texture = XMFLOAT2(1.f, 1.f); 

	vertices[2].position = XMFLOAT3(-width, -height, 0.f); 
	vertices[2].texture = XMFLOAT2(0.f, 1.f); 

	// Second triangle. 
	vertices[3].position = XMFLOAT3(-width, height, 0.f); // Top Left
	vertices[3].texture = XMFLOAT2(0.f, 0.f); 

	vertices[4].position = XMFLOAT3(width, height, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(width, -height, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data. 
	for (i = 0; i < m_indexCount; ++i)
	{
		indices[i] = i; 
	}

	// Setup the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0; 
	vertexBufferDesc.StructureByteStride = 0; 

	// Give the subresource structure a pointer to the vertex data. 
	vertexData.pSysMem = vertices; 
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0; 

	// Now finally create the vertex buffer. 
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) { return false; }

	// Setup the description of the index buffer. 
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount; 
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	indexBufferDesc.CPUAccessFlags = 0; 
	indexBufferDesc.MiscFlags = 0; 
	indexBufferDesc.StructureByteStride = 0; 

	// Give the subresource strucutre a pointer to the vertex data. 
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

void DisplayPlaneClass::ShutdownBuffers()
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

void DisplayPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	return; 
}
