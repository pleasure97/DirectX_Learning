#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


/* INCLUDES */
#include <directxmath.h>
using namespace DirectX; 

/* MY CLASS INCLUDES */
#include "textureclass.h"

class BitmapClass
{
private:

	struct VertexType
	{
		XMFLOAT3 position; 
		XMFLOAT2 texture; 
	};

public:
	BitmapClass(); 
	BitmapClass(const BitmapClass&); 
	~BitmapClass(); 

	bool Initilaize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int); 
	void Shutdown(); 
	bool Render(ID3D11DeviceContext*); 

	int GetIndexCount(); 
	ID3D11ShaderResourceView* GetTexture(); 

	void SetRenderLocation(int, int); 

private:
	bool InitializeBuffers(ID3D11Device*); 
	void ShutdownBuffers(); 
	bool UpdateBuffers(ID3D11DeviceContext*); 
	void RenderBuffers(ID3D11DeviceContext*); 

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*); 
	void ReleaseTexture(); 

private:
	ID3D11Buffer* m_vertexBuffer; 
	ID3D11Buffer* m_indexBuffer; 
	int m_vertexCount; 
	int m_indexCount;
	int m_screenWidth, m_screenHeight; 
	int m_bitmapWidth, m_bitmapHeight; 
	int m_renderX, m_renderY;
	int m_prevPosX, m_prevPosY; 
	TextureClass* m_Texture; 
};


#endif 