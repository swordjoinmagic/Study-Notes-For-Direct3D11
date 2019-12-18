#pragma once

#include "d3dApp.h"
#include "D3DUtils.h"
#include "Shader.h"
#include "Texture.h"

class Sample8 : public D3DApp{
private:
	std::shared_ptr<Shader> treeShader;
	std::shared_ptr<Texture> treeTexture;
	
	// ¶¥µã/Ë÷Òý»º³å
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11InputLayout* inputLayout;
public:
	Sample8(HINSTANCE hInstance):D3DApp(hInstance) {}
	~Sample8() {
		ReleaseCOM(vertexBuffer);
		ReleaseCOM(indexBuffer);
		ReleaseCOM(inputLayout);
	}
	void OnStart() override;
	void UpdateScene(float deltaTime) override;
	void Render() override;
};