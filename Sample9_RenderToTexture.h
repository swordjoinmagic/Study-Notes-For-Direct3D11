#pragma once

#include "d3dApp.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Texture.h"
#include "Light.h"

class Sample9RenderToTexture : public D3DApp{
private:
	// ������Ⱦ������,���ں���
	ID3D11Texture2D* offScreenTex;
	// ����������ͼ
	ID3D11RenderTargetView* offScreenTexRT;
	// ����������ɫ����ͼ
	ID3D11ShaderResourceView* offScreenTexSRV;

	GeometryGenerator geogen;

	Light light;

	// ��������
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Texture> boxTex;
	std::shared_ptr<Texture> planeTex;
	std::shared_ptr<Shader> texShader;

	// ����shader
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