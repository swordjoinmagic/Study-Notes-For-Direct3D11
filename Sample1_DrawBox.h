#pragma once

#include "d3dApp.h"

struct Vertex {
	float3 pos;
	float4 Color;
};

class Sample1 : public D3DApp{
private:
	// ¶¥µã»º³åºÍË÷Òý»º³å
	ID3D11Buffer* boxVB;
	ID3D11Buffer* boxIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxMVP;

	ID3D11InputLayout* mInputLayout;

	float4x4 mModel;
	float4x4 mView;
	float4x4 mProj;

public:
	Sample1(HINSTANCE hInstance);
	~Sample1();
	void OnStart() override;
	void Render() override;
	void UpdateScene(float deltaTime) override;
};