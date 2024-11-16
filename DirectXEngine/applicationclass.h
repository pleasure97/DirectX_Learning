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
#include "xinputclass.h"
#include "cameraclass.h"
#include "fontshaderclass.h"
#include "fontclass.h"
#include "textclass.h"

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
	bool Render(); 
	bool UpdateControllerStrings(); 
private:
	D3DClass* m_Direct3D; 
	XInputClass* m_XInput;
	CameraClass* m_Camera; 
	FontShaderClass* m_FontShader;
	FontClass* m_Font; 
	TextClass* m_ActiveStrings; 
	TextClass* m_ButtonStrings; 
	TextClass* m_TriggerStrings; 
	TextClass* m_ThumbStrings; 
};
#endif