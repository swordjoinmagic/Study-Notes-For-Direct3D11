#pragma once

#include "d3dApp.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Texture.h"
#include "Light.h"

class Sample9RenderToTexture : public D3DApp{
private:
	// 离屏渲染的纹理,用于后处理
	ID3D11Texture2D* offScreenTex;
	// 离屏纹理视图
	ID3D11RenderTargetView* offScreenTexRT;
	// 离屏纹理着色器视图
	ID3D11ShaderResourceView* offScreenTexSRV;

	GeometryGenerator geogen;

	Light light;

	// 场景物体
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Texture> boxTex;
	std::shared_ptr<Texture> planeTex;
	std::shared_ptr<Shader> texShader;

	// 后处理shader
	std::shared_ptr<Shader> postEffectShader;
	std::shared_ptr<Mesh> quadMesh;

public:
	Sample9RenderToTexture(HINSTANCE hInstance) : D3DApp(hInstance) {};
	~Sample9RenderToTexture() {
		ReleaseCOM(offScreenTex);
		ReleaseCOM(offScreenTexRT);
		ReleaseCOM(offScreenTexSRV);
	}
	void OnStart() override;
	void DrawScene() override;
	void Render() override;
	void InitTexView();
	void DrawGameScene();
	void DrawQuad();
};