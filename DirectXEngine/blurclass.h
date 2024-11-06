#ifndef _BLURCLASS_H_
#define _BLURCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "rendertextureclass.h"
#include "orthowindowclass.h"
#include "textureshaderclass.h"
#include "blurshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: BlurClass
////////////////////////////////////////////////////////////////////////////////
class BlurClass
{
public:
    BlurClass();
    BlurClass(const BlurClass&);
    ~BlurClass();

    bool Initialize(D3DClass*, int, int, float, float, int, int);
    void Shutdown();

    bool BlurTexture(D3DClass*, CameraClass*, RenderTextureClass*, TextureShaderClass*, BlurShaderClass*);

private:
    RenderTextureClass* m_DownSampleTexture1;
    RenderTextureClass* m_DownSampleTexture2;
    OrthoWindowClass* m_DownSampleWindow;
    OrthoWindowClass* m_UpSampleWindow;
    int m_downSampleWidth;
    int m_downSampleHeight;
};

#endif 