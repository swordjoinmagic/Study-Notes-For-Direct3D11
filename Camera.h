#pragma once

#include "D3DUtils.h"

class Camera {
private:
	// 以弧度为单位
	float fov;
	float3 forward;
public:
	float3 pos;
	float3 rotation;
	// 以角度为单位
	float fovAngle;
	float Near, Far;
	float aspect;
	Camera(float3 _pos,float3 _rotation,float _fovAngle,float _near,float _far,float _aspect):
	pos(_pos),rotation(_rotation),fovAngle(_fovAngle),Near(_near),Far(_far),aspect(_aspect),forward(0,0,1),fov(fovAngle*3.1415926f/180.0f){}
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjMatrix();
};