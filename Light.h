#pragma once

#include "D3DUtils.h"

struct Light {
	float3 pos;	// 平行光不使用
	float Kc;	// 衰减常数项
	// 用作平行光时为平行光方向,
	// 用作聚光灯时,表示聚光灯朝向方向spotDir
	float3 dir;	
	float KI;	// 衰减一次项
	float3 lightColor;
	float Kq;	// 衰减二次项
	// φ,聚光灯外角
	float Phi;
	// θ,聚光灯内角,
	float Theta;

	XMVECTOR GetDir(float3 objPos) {
		XMVECTOR lpos = XMLoadFloat3(&pos); 
		XMVECTOR opos = XMLoadFloat3(&objPos);
		return lpos - opos;
	}
public:
	Light() { ZeroMemory(this, sizeof(this)); }
};