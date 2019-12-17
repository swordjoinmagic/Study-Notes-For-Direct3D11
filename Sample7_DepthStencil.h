#pragma once

#include "d3dApp.h"
#include "D3DUtils.h"
#include "Shader.h"
#include "Mesh.h"
#include "GeometryGenerator.h"
#include "Texture.h"
#include "Light.h"

class Sample7 : public D3DApp{
private:
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Shader> shader;
	
	std::shared_ptr<Texture> mirrorTex;
	std::shared_ptr<Texture> boxTex;
	std::shared_ptr<Texture> planeTex;

	GeometryGenerator geoGem;

	Light light;

	ID3D11BlendState* blendState;
	ID3D11DepthStencilState* doNotWriteDepth;
	ID3D11DepthStencilState* stencilTest;
	ID3D11BlendState* doNotRenderTarget;

	float3 boxPos;
public:
	Sample7(HINSTANCE hInstance) :D3DApp(hInstance) {
		boxPos = float3(0,3,0);
	}
	~Sample7() {
		ReleaseCOM(blendState);
		ReleaseCOM(doNotWriteDepth);
		ReleaseCOM(stencilTest);
		ReleaseCOM(doNotRenderTarget);
	}

	void OnStart() override;
	void UpdateScene(float deltaTime) override;
	void Render() override;
	void ProcessKeyBoardInput() override;

};