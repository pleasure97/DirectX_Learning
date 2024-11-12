#include "heatclass.h"

HeatClass::HeatClass()
{
	m_HeatNoiseTexture = 0; 
}

HeatClass::HeatClass(const HeatClass&)
{
}

HeatClass::~HeatClass()
{
}

bool HeatClass::Initialize(D3DClass* Direct3D)
{
	char textureFilename[128]; 
	bool result;

	// Create and initialize the heat noise texture object.
	m_HeatNoiseTexture = new TextureClass;

	strcpy_s(textureFilename, "../DirectXEngine/data/heatnoise01.tga"); 

	result = m_HeatNoiseTexture->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), textureFilename); 
	if (!result)
	{
		return false;
	}

	// Set the 3 scrolling speeds for the 3 different noise textures.
	m_scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f); 
	
	// Set the 3 scales which will be used to create the 3 different noise octave textures.
	m_scales = XMFLOAT3(1.f, 2.f, 3.f); 

	// Set the 3 different x and y distortion factors for the 3 different noise textures.
	m_distortion1 = XMFLOAT2(0.1f, 0.2f); 
	m_distortion2 = XMFLOAT2(0.1f, 0.3f); 
	m_distortion3 = XMFLOAT2(0.1f, 0.1f); 

	// Set the scale and bias of the texture coordinate sampling perturbation.
	m_distortionScale = 0.8f; 
	m_distortionBias = 0.5f; 
	m_emissiveMultiplier = 1.6f; 

	// Initialize the noise timing.
	m_noiseFrameTime = 0.f;

	return true; 
}

void HeatClass::Shutdown()
{
	// Release the heat noise texture.
	if (m_HeatNoiseTexture)
	{
		m_HeatNoiseTexture->Shutdown();
		delete m_HeatNoiseTexture;
		m_HeatNoiseTexture = 0;
	}

	return;
}

void HeatClass::Frame(float frameTime)
{
	// Increment the frame time counter. 
	m_noiseFrameTime += (frameTime * 0.075f);
	if (m_noiseFrameTime > 1000.f)
	{
		m_noiseFrameTime = 0.f;
	}

	return; 
}

ID3D11ShaderResourceView* HeatClass::GetTexture()
{
	return m_HeatNoiseTexture->GetTexture(); 
}

void HeatClass::GetNoiseValues(XMFLOAT3& scrollSpeeds, XMFLOAT3& scales,
	XMFLOAT2& distortion1, XMFLOAT2& distortion2, XMFLOAT2& distortion3, float& distortionScale, float& distortionBias, 
	float& emissiveMultiplier, float& noiseFrameTime)
{
	scrollSpeeds = m_scrollSpeeds;
	scales = m_scales;
	distortion1 = m_distortion1;
	distortion2 = m_distortion2;
	distortion3 = m_distortion3;
	distortionScale = m_distortionScale;
	distortionBias = m_distortionBias;
	emissiveMultiplier = m_emissiveMultiplier;
	noiseFrameTime = m_noiseFrameTime;

	return;
}
