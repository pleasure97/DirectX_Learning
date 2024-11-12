#include "parallaxscrollclass.h"

ParallaxScrollClass::ParallaxScrollClass()
{
	m_TextureArray = 0;
	m_ParallaxArray = 0;
}

ParallaxScrollClass::ParallaxScrollClass(const ParallaxScrollClass&)
{
}

ParallaxScrollClass::~ParallaxScrollClass()
{
}

bool ParallaxScrollClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* configFilename)
{
	ifstream fin; 
	char textureFilename[256]; 
	int i, j; 
	char input;
	float scrollSpeed; 
	bool result;

	// Open the file.
	fin.open(configFilename); 
	if (!fin.good())
	{
		return false;
	}

	// Read the number of textures in the file.
	fin.get(input); 
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_textureCount; 

	// Read the end of the line.
	fin.get(input); 

	// Boundary check.
	if ((m_textureCount < 1) || (m_textureCount > 64))
	{
		return false;
	}
	
	// Create and initialize the texture object array.
	m_TextureArray = new TextureClass[m_textureCount]; 

	// Create and initialize the parallax struct array.
	m_ParallaxArray = new ParallaxArrayType[m_textureCount]; 

	// Loop through each line containing a texture filename and a scroll speed.
	for (i = 0; i < m_textureCount; i++)
	{
		j = 0; 
		fin.get(input); 

		// Read in the texture file name.
		while (input != ' ')
		{
			textureFilename[i] = input; 
			j++; 
			fin.get(input);
		}
		textureFilename[j] = '\0';

		// Read in the scroll speed.
		fin >> scrollSpeed; 

		// Read end of line character.
		fin.get(input); 

		// Load the texture into the texture array.
		result = m_TextureArray[i].Initialize(device, deviceContext, textureFilename); 
		if (!result)
		{
			return false; 
		}

		// Store the scroll speed and initialize the translation of the texture at the start.
		m_ParallaxArray[i].scrollSpeed = scrollSpeed; 
		m_ParallaxArray[i].textureTranslation = 0.f; 
	}

	// Close the file.
	fin.close(); 

	return true; 
}

void ParallaxScrollClass::Shutdown()
{
	int i; 

	// Release the parallax struct array.
	if (m_ParallaxArray)
	{
		delete[] m_ParallaxArray;
		m_ParallaxArray = 0; 
	}

	// Release the texture object array.
	if (m_TextureArray)
	{
		for (i = 0; i < m_textureCount; i++)
		{
			m_TextureArray[i].Shutdown(); 
		}

		delete[] m_TextureArray;
		m_TextureArray = 0; 
	}

	return;
}

void ParallaxScrollClass::Frame(float frameTime)
{
	int i; 

	// Loop through each of elements in the parallax struct array.
	for (i = 0; i < m_textureCount; i++)
	{
		// Increment the texture translation by the frame time each frame.
		m_ParallaxArray[i].textureTranslation += frameTime * m_ParallaxArray[i].scrollSpeed;
		if (m_ParallaxArray[i].textureTranslation > 1.f)
		{
			m_ParallaxArray[i].textureTranslation -= 1.f; 
		}
	}

	return;
}

int ParallaxScrollClass::GetTextureCount()
{
	return m_textureCount; 
}

ID3D11ShaderResourceView* ParallaxScrollClass::GetTexture(int index)
{
	return m_TextureArray[index].GetTexture();
}

float ParallaxScrollClass::GetTranslation(int index)
{
	return m_ParallaxArray[index].textureTranslation;
}
