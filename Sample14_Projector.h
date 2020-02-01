#pragma once

#include "d3dApp.h"
#include "Mesh.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Texture.h"

// 投影器的简单示例
class Sample14 : public D3DApp{
private:
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Shader> shader;

	std::shared_ptr<Texture> mainTex;
	std::shared_ptr<Texture> projTex;

	GeometryGenerator geogen;

	float3 projectorPos;
public:
	Sample14(HINSTANCE hInstance) :D3DApp(hInstance) {}
	virtual ~Sample14() {}
	void OnStart() override;
	void Render() override;
	void UpdateScene(float deltaTime) override;
};