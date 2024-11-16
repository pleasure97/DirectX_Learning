#include "particlesystemclass.h"

ParticleSystemClass::ParticleSystemClass()
{
	m_vertexBuffer = 0; 
	m_indexBuffer = 0; 
	m_particleList = 0; 
	m_vertices = 0; 
	m_Texture = 0; 
}

ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass&)
{
}

ParticleSystemClass::~ParticleSystemClass()
{
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* configFilename)
{
	bool result; 

	// Keep a copy of the config file name for loading the particle configuration, and also for mid-app learning.
	strcpy_s(m_configFilename, configFilename); 
		
	// Load the particle configuration file to set all the particle parameters for rendering.
	result = LoadParticleConfiguration(); 
	if (!result)
	{
		return false; 
	}

	// Initialize the particle system.
	result = InitializeParticleSystem(); 
	if (!result)
	{
		return false; 
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device); 
	if (!result)
	{
		return false; 
	}

	// Load the texture that is used for the particles.
	result = LoadTexture(device, deviceContext); 
	if (!result)
	{
		return false;
	}

	return true; 
}

void ParticleSystemClass::Shutdown()
{
	// Release the texture used for the particles.
	ReleaseTexture(); 

	// Release the buffers.
	ShutdownBuffers(); 

	// Release the particle system.
	ShutdownParticleSystem(); 

	return;
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result; 

	// Release old particles.
	KillParticles(); 

	// Emit new particles.
	EmitParticles(frameTime); 

	// Update the position of the particles.
	UpdateParticles(frameTime); 

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext); 
	if (!result)
	{
		return false;
	}
	
	return true; 
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext); 

	return;
}

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return m_Texture->GetTexture(); 
}

int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}

bool ParticleSystemClass::LoadParticleConfiguration()
{
	ifstream fin;
	int i; 
	char input; 

	// Open the particle configuration file.
	fin.open(m_configFilename); 
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of the particle count and read it in.
	fin.get(input); 
	while (input != ':')
	{
		fin.get(input); 
	}
	fin >> m_maxParticles; 

	// Read up to the value of the particle per second and read it in.
	fin.get(input); 
	while (input != ':')
	{
		fin.get(input); 
	}
	fin >> m_particlesPerSecond; 

	// Read up to the value of the particle life time and read it in.
	fin.get(input); 
	while (input != ':')
	{
		fin.get(input); 
	}
	fin >> m_particleLifeTime; 

	// Read up to the filename of the first texture and read it in.
	fin.get(input); 
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);

	i = 0; 
	fin.get(input); 
	while (input != '\n')
	{
		m_textureFilename[i] = '\0';
		i++; 
		fin.get(input); 
	}
	m_textureFilename[i] = '\0';

	// Close the file
	fin.close(); 

	return true; 
}

bool ParticleSystemClass::InitializeParticleSystem()
{
	int i; 

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles]; 

	// Initialize the particle list.
	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.f; 

	// Initialize the current particle count to 0 since none are emitted yet. 
	m_currentParticleCount = 0; 

	return true; 
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	// Release the particle list.
	if (m_particleList)
	{
		delete[] m_particleList; 
		m_particleList = 0; 
	}

	return; 
}

void ParticleSystemClass::ShutdownBuffers()
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

	// Release the vertices.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}

	return;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride; 
	unsigned int offset; 

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0; 

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset); 

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0); 

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 

	return; 
}

void ParticleSystemClass::EmitParticles(float frameTime)
{
	float centerX, centerY; 
	float radius; 
	float positionX, positionY, positionZ; 
	float scroll1X, scroll1Y; 
	float emitterOrigin[3]; 
	int index, i, j;
	bool emitParticle; 
	bool found; 
	static float angle = 0.f; 

	// Set the center of the circle.
	centerX = 0.f; 
	centerY = 0.f; 

	// Set the radius of the circle.
	radius = 1.f; 

	// Update the angle each frame to move any generated origin position along the circumfence of the circle each frma.e
	angle += frameTime * 2.f; 

	// Calculate the origin that the particle should be emitted on the circle's circumfence.
	emitterOrigin[0] = centerX + radius * sin(angle); 
	emitterOrigin[1] = centerY + radius * cos(angle); 
	emitterOrigin[2] = 0.f; 

	// Increment the accumulated time that is used to determine when to emit a particle next.
	m_accumulatedTime += frameTime; 

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1.f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.f; 
		emitParticle = true; 
	}

	// If there are particles to emit, then emit one per frame.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++; 
	}

	positionX = emitterOrigin[0]; 
	positionY = emitterOrigin[1]; 
	positionZ = emitterOrigin[2]; 

	// Create a random X scrolling positive value.
	scroll1X = (((float)rand() - (float)rand()) / RAND_MAX); 
	if (scroll1X < 0.f)
	{
		scroll1X *= -1.f;
	}

	// Set the Y scroll to the same value.
	scroll1Y = scroll1X;

	// Now since the particles need to be rendered from back to front for blending, we have to sort the particle array.
	// We will sort using Z depth so we need to find where in the list in the particle should be inserted.
	index = 0; 
	found = false; 
	while (!found)
	{
		if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
		{
			found = true; 
		}
		else
		{
			index++; 
		}
	}

	// Now that we know the location to insert into, we need to copy the array over by one position from the index to make room for the new particle.
	i = m_currentParticleCount; 
	j = i - 1; 

	while (i != index)
	{
		CopyParticle(i, j); 
		i--; 
		j--; 
	}

	// Now insert the newly emitted particle into the particle array in the correct depth order.
	m_particleList[index].positionX = positionX; 
	m_particleList[index].positionY = positionY; 
	m_particleList[index].positionZ = positionZ; 
	m_particleList[index].active = true; 
	m_particleList[index].lifeTime = m_particleLifeTime; 
	m_particleList[index].scroll1X = scroll1X; 
	m_particleList[index].scroll1Y = scroll1Y; 

	return; 
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{
	int i; 

	// Each frame we update all the particles using the frame time.
	for (i = 0; i < m_currentParticleCount; i++)
	{
		// Negate the life time of the particle each frame.
		m_particleList[i].lifeTime -= frameTime; 

		// Update the scrolling position of each particle each frame.
		m_particleList[i].scroll1X += frameTime * 0.5f; 
		if (m_particleList[i].scroll1X > 1.f)
		{
			m_particleList[i].scroll1X -= 1.f;
		}

		m_particleList[i].scroll1Y += frameTime * 0.5f; 
		if (m_particleList[i].scroll1Y > 1.f)
		{
			m_particleList[i].scroll1Y -= 1.f; 
		}
	}
	
	return; 
}

void ParticleSystemClass::KillParticles()
{
	int i, j;

	// Kill all the particles that have a lifetime that is now 0.
	for (i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].lifeTime < 0.f))
		{
			m_particleList[i].active = false; 
			m_currentParticleCount--; 

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < m_maxParticles; j++)
			{
				CopyParticle(j, j + 1); 
			}
		}
	}

	return; 
}

void ParticleSystemClass::CopyParticle(int dst, int src)
{
	m_particleList[dst].positionX = m_particleList[src].positionX; 
	m_particleList[dst].positionY = m_particleList[src].positionY; 
	m_particleList[dst].positionZ = m_particleList[src].positionZ;
	m_particleList[dst].active = m_particleList[src].active; 
	m_particleList[dst].lifeTime = m_particleList[src].lifeTime;
	m_particleList[dst].scroll1X = m_particleList[src].scroll1X;
	m_particleList[dst].scroll1Y = m_particleList[src].scroll1Y;

	return;
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices; 
	int i; 
	D3D11_BUFFER_DESC vertexBufferDesc; 
	D3D11_BUFFER_DESC indexBufferDesc; 
	D3D11_SUBRESOURCE_DATA vertexData; 
	D3D11_SUBRESOURCE_DATA indexData; 
	HRESULT result; 

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array. 
	m_indexCount = m_vertexCount; 

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount]; 

	// Create the index array. 
	indices = new unsigned long[m_indexCount]; 

	// Initialize the vertex array to 0s at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount)); 

	// Initialize the index array.
	for (i = 0; i < m_indexCount; i++)
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
	vertexData.pSysMem = m_vertices; 
	vertexData.SysMemPitch = 0; 
	vertexData.SysMemSlicePitch = 0; 

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); 
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
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
	if (FAILED(result))
	{
		return false; 
	}

	// Release just the index array since it's no longer needed.
	delete[] indices; 
	indices = 0; 
	
	return true;
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i; 
	HRESULT result; 
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	VertexType* verticesPtr; 
	float lifeTime; 
	float scroll1X, scroll1Y; 

	// Initialize the vertex array to 0s at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount)); 

	// Now build the vertex array from the particle list array. 
	// Each particle is a quad made out of 2 triangles.
	index = 0; 

	for (i = 0; i < m_currentParticleCount; i++)
	{
		// Get the liftime and scroll for the current particle.
		// This will be set in the data1 portion of the vertex.
		lifeTime = m_particleList[i].lifeTime / m_particleLifeTime; 
		scroll1X = m_particleList[i].scroll1X; 
		scroll1Y = m_particleList[i].scroll1Y; 

		// Bottom Left. 
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.f, 1.f); 
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++; 

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.0f);
		index++;
	}

	// Lock the vertex buffer. 
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); 
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData; 

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount)); 

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0); 

	return true; 
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result; 

	// Create and initialize the texture object.
	m_Texture = new TextureClass; 

	result = m_Texture->Initialize(device, deviceContext, m_textureFilename); 
	if (!result)
	{
		return false;
	}

	return true; 
}

void ParticleSystemClass::ReleaseTexture()
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

bool ParticleSystemClass::Reload(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result; 

	// Release all of the data.
	Shutdown(); 

	// Reload all the data.
	result = LoadParticleConfiguration(); 
	if (!result)
	{
		return false;
	}

	result = InitializeParticleSystem(); 
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device); 
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext); 
	if (!result)
	{
		return false;
	}

	return true; 
}