#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0; 
	m_Texture = 0; 
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int fontChoice)
{
	char fontFilename[128]; 
	char fontTextureFilename[128]; 
	bool result; 

	// Choose one of the available fonts, and default to the first font otherwise. 
	switch (fontChoice)
	{
		case 0:
		{
			strcpy_s(fontFilename, "../DirectXEngine/data/font/font01.txt");
			strcpy_s(fontTextureFilename, "../DirectXEngine/data/font/font01.tga"); 
			m_fontHeight = 32.f;
			m_spaceSize = 3; 
			break;
		}
		default:
		{
			strcpy_s(fontFilename, "../DirectXEngine/data/font/font01.txt");
			strcpy_s(fontTextureFilename, "../DirectXEngine/data/font/font01.tga"); 
			m_fontHeight = 32.f; 
			m_spaceSize = 3;
			break;
		}
	}

	// Load in the text file containing the font data. 
	result = LoadFontData(fontFilename); 
	if (!result) { return false; }

	// Load the texture that has the font characters on it. 
	result = LoadTexture(device, deviceContext, fontTextureFilename); 
	if (!result) { return false; }
	

	return true;
}

void FontClass::Shutdown()
{
	// Release the font texture. 
	ReleaseTexture(); 

	// Release the font data.
	ReleaseFontData(); 

	return; 
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr; 
	int numLetters; 
	int index; 
	int i; 
	int letter; 

	// Coerce the input vertices into a VertexType structure. 
	vertexPtr = (VertexType*)vertices; 

	// Get the number of letters in the sentence. 
	numLetters = (int)strlen(sentence); 

	// Initialize the index to the vertex array. 
	index = 0; 

	// Draw each letter onto a quad. 
	for (i = 0; i < numLetters; ++i)
	{
		letter = ((int)sentence[i]) - 32; 

		// If the letter is a space, then just move over 3 pixels. 
		if (letter == 0)
		{
			drawX += m_spaceSize; 
		}
		else
		{
			// First triangle in quad. 
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.f); // Top Left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.f); 
			index++; 

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.f); // Bottom Right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.f); 
			index++; 

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.f); // Bottom Left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.f); 
			index++; 

			// Second triangle in quad. 
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.f); 
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.f); 
			index++; 

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.f); 
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.f);
			index++; 

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.f); 
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.f); 
			index++; 

			// Update the x location for drawing by the size of the letter and the pixel. 
			drawX += m_Font[letter].size + 1.f; 
		}
	}

	return; 
}

int FontClass::GetSentencePixelLength(char* sentence)
{
	int pixelLength;
	int numLetters;
	int i; 
	int letter; 

	pixelLength = 0; 
	numLetters = (int)strlen(sentence); 

	for (i = 0; i < numLetters; ++i)
	{
		letter = ((int)sentence[i]) - 32; 

		// If the letter is a space then count it as 3 pixels.
		if (letter == 0)
		{
			pixelLength += m_spaceSize; 
		}
		else
		{
			pixelLength += (m_Font[letter].size + 1); 
		}
	}
	return pixelLength;
}

int FontClass::GetFontHeight()
{
	return (int)m_fontHeight;
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin; 
	int i; 
	char temp; 

	// Create the font spacing buffer.
	m_Font = new FontType[95]; 

	// Read the font size and spacing between chars. 
	fin.open(filename); 
	if (fin.fail()) { return false; }

	// Read in the 95 used ascii characters for text. 
	for (i = 0; i < 95; ++i)
	{
		fin.get(temp); 
		while (temp != ' ')
		{
			fin.get(temp); 
		}
		fin.get(temp); 
		while (temp != ' ')
		{
			fin.get(temp); 
		}

		fin >> m_Font[i].left; 
		fin >> m_Font[i].right; 
		fin >> m_Font[i].size; 
	}

	// Close the file. 
	fin.close(); 

	return true; 
}

void FontClass::ReleaseFontData()
{
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result; 

	// Create and initialize the font texture object. 
	m_Texture = new TextureClass; 

	result = m_Texture->Initialize(device, deviceContext, filename); 
	if (!result) { return false; }

	return true; 
}

void FontClass::ReleaseTexture()
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
