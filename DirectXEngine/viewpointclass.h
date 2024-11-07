#ifndef _VIEWPOINTCLASS_H_
#define _VIEWPOINTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ViewPointClass
////////////////////////////////////////////////////////////////////////////////
class ViewPointClass
{
public:
    ViewPointClass();
    ViewPointClass(const ViewPointClass&);
    ~ViewPointClass();

    void SetPosition(float, float, float); 
    void SetLookAt(float, float, float);
    void SetProjectionParameters(float, float, float, float); 

    void GenerateViewMatrix(); 
    void GenerateProjectionMatrix(); 

    void GetViewMatrix(XMMATRIX&); 
    void GetProjectionMatrix(XMMATRIX&); 

private:
    XMMATRIX m_viewMatrix;
    XMMATRIX m_projectionMatrix; 
    XMFLOAT3 m_position;
    XMFLOAT3 m_lookAt; 
    float m_fieldOfView; 
    float m_aspectRatio; 
    float m_nearPlane; 
    float m_farPlane; 

};

#endif 
