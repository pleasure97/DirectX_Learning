#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_

/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")

//////////////
// INCLUDES //
//////////////
#include <Windows.h>
#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass(); 
	FpsClass(const FpsClass&); 
	~FpsClass(); 

	void Initialize(); 
	void Frame(); 
	int GetFps(); 

private:
	int m_fps; 
	int m_count; 
	unsigned long m_startTime; 
};



#endif
