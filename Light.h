#pragma once

#include "D3DUtils.h"

struct Light {
	float3 pos;	// ƽ�йⲻʹ��
	float Kc;	// ˥��������
	// ����ƽ�й�ʱΪƽ�йⷽ��,
	// �����۹��ʱ,��ʾ�۹�Ƴ�����spotDir
	float3 dir;	
	float KI;	// ˥��һ����
	float3 lightColor;
	float Kq;	// ˥��������
	// ��,�۹�����
	float Phi;
	// ��,�۹���ڽ�,
	float Theta;

	XMVECTOR GetDir(float3 objPos) {
		XMVECTOR lpos = XMLoadFloat3(&pos); 
		XMVECTOR opos = XMLoadFloat3(&objPos);
		return lpos - opos;
	}
public:
	Light() { ZeroMemory(this, sizeof(this)); }
};