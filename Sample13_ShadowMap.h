#pragma once

#include "d3dApp.h"
#include "Mesh.h"
#include "Texture.h"
#include "GeometryGenerator.h"
#include "Light.h"

class Sample13 : public D3DApp{
private:
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Mesh> quadMesh;
	std::shared_ptr<Mesh> lightMesh;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> createShadowMapShader;	
	std::shared_ptr<Shader> postEffectShader;
	std::shared_ptr<Shader> lightShader;
	
	std::shared_ptr<Texture> boxTexture;
	std::shared_ptr<Texture> planeTexture;

	ComPtr<ID3D11Texture2D> offScreenTex;
	ComPtr<ID3D11RenderTargetView> offScreenTexRTV;
	ComPtr<ID3D11ShaderResourceView> offScreenTexSRV;

	ComPtr<ID3D11Texture2D> shadowMap;
	ComPtr<ID3D11ShaderResourceView> shadowMapSRV;
	ComPtr<ID3D11DepthStencilView> shadowMapDepthView;

	ComPtr<ID3D11InputLayout> shaderIL;
	ComPtr<ID3D11InputLayout> createShadowMapShaderIL;

	std::vector<float3> posArray;
	GeometryGenerator geogen;

	// 平行光光源
	Light light;

	float bias = 0.005f;
	
public:
	Sample13(HINSTANCE hInstance) : D3DApp(hInstance){}
	virtual ~Sample13() override {};
	void OnStart() override;
	void Render() override;
	void UpdateScene(float deltaTime) override;
	void RenderScene(XMMATRIX LightView, XMMATRIX LightProj, Shader* shader, ID3D11InputLayout* inputLayout);
	void RenderOffScreen();
	void RenderSceneToCreateShadowMap(XMMATRIX view, XMMATRIX proj);
	
};