#pragma once
#include "GeometryGenerator.h"
#include "d3dApp.h"
#include "Light.h"

class Sample4 : public D3DApp{
private:

	struct Vertex {
		float3 pos;
		float3 normal;
	};

	ID3D11Buffer* objectIndexBuffer;
	ID3D11Buffer* objectVertexBuffer;

	ID3DX11Effect* effect;
	ID3DX11EffectTechnique* technique;
	ID3DX11EffectMatrixVariable* fxMvp;

	GeometryGenerator geogen;

	ID3D11InputLayout* inputLayout;

	Light parallelLight;
	

	uint indexCount;

public:
	Sample4(HINSTANCE hInstance);
	~Sample4();
	void OnStart() override;
	void Render() override;
	void UpdateScene(float deltaTime) override;
};