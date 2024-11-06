////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "inputclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "rendertextureclass.h"
#include "orthowindowclass.h"
#include "blurclass.h"
#include "blurshaderclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool RenderSceneToTexture(float);
	bool Render(); 

private:
	D3DClass* m_Direct3D; 
	CameraClass* m_Camera; 
	TextureShaderClass* m_TextureShader;
	ModelClass* m_Model; 
	RenderTextureClass* m_RenderTexture; 
	OrthoWindowClass* m_FullScreenWindow; 
	BlurClass* m_Blur; 
	BlurShaderClass* m_BlurShader; 
};

#endif