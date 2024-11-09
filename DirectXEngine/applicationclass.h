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
#include "lightclass.h"
#include "lightshaderclass.h"
#include "fontshaderclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "bitmapclass.h"
#include "textureshaderclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_NEAR = 0.3f;
const float SCREEN_DEPTH = 1000.f;

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
	bool RenderGlowToTexture(float); 
	bool Render(); 

	bool TestIntersection(int, int); 
	bool RaySphereIntersect(XMFLOAT3, XMFLOAT3, float); 

private:
	D3DClass* m_Direct3D; 
	CameraClass* m_Camera; 
	ModelClass* m_Model;
	LightClass* m_Light; 
	LightShaderClass* m_LightShader;
	FontShaderClass* m_FontShader;
	FontClass* m_Font; 
	TextClass* m_TextString; 
	BitmapClass* m_MouseBitmap; 
	TextureShaderClass* m_TextureShader;
	int m_screenWidth; 
	int m_screenHeight; 
};

#endif