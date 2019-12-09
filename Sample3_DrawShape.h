#pragma once
#include "d3dApp.h"
#include "GeometryGenerator.h"

class Sample3 : public D3DApp{
private:
	GeometryGenerator geoGen;

	ID3D11Buffer* shapeVertexBuffer;
	ID3D11Buffer* shapeIndexBuffer;

	ID3D11InputLayout* inputLayout;

	ID3DX11Effect* effect;
	ID3DX11EffectTechnique* technique;
	ID3DX11EffectMatrixVariable* fxmvp;

	uint indexCount;
public:
	Sample3(HINSTANCE hInstance);
	~Sample3();
	void OnStart() override;
	void Render() override;
public:
	struct Vertex {
	public:
		float3 pos;
	};
};