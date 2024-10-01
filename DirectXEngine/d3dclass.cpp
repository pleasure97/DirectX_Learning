#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0; 
	m_device = 0; 
	m_deviceContext = 0; 
	m_renderTargetView = 0; 
	m_depthStencilBuffer = 0; 
	m_depthStencilState = 0; 
	m_depthStencilView = 0; 
	m_rasterState = 0; 
}

D3DClass::D3DClass(const D3DClass&)
{
}

D3DClass::~D3DClass()
{
}
