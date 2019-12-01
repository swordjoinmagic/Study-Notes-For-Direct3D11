#pragma once

#include "d3dApp.h"
#include "GeometryGenerator.h"

class Sample2 : public D3DApp{
private:
	// ¶¥µã»º³åºÍË÷Òý»º³å
	ID3D11Buffer* hilsVB;
	ID3D11Buffer* hilsIB;

	ID3DX11Effect* fx;
	ID3DX11EffectTechnique* tech;
	ID3DX11EffectMatrixVariable* fxmvp;

	ID3D11InputLayout* inputLayout;

	GeometryGenerator geogen;

	uint indexCount;
public:
	struct Vertex {
		float3 pos;
		float4 color;
	};
	Sample2(HINSTANCE hInstance);
	~Sample2();
	void OnStart() override;
	void Render() override;
	float GetHeight(float x,float y);
};