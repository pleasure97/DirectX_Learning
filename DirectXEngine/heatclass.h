#ifndef _HEATCLASS_H_
#define _HEATCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: HeatClass
////////////////////////////////////////////////////////////////////////////////
class HeatClass
{
public:
    HeatClass();
    HeatClass(const HeatClass&);
    ~HeatClass();

    bool Initialize(D3DClass*);
    void Shutdown();
    void Frame(float);

    ID3D11ShaderResourceView* GetTexture();
    void GetNoiseValues(XMFLOAT3&, XMFLOAT3&, XMFLOAT2&, XMFLOAT2&, XMFLOAT2&, float&, float&, float&, float&);

private:
    TextureClass* m_HeatNoiseTexture;
    XMFLOAT3 m_scrollSpeeds;
    XMFLOAT3 m_scales;
    XMFLOAT2 m_distortion1; 
    XMFLOAT2 m_distortion2;
    XMFLOAT2 m_distortion3; 
    float m_distortionScale;
    float m_distortionBias; 
    float m_emissiveMultiplier; 
    float m_noiseFrameTime; 

};
#endif 