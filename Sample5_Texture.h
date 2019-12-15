#pragma once

#include "d3dApp.h"
#include "D3DUtils.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "GeometryGenerator.h"

class Sample5 : public D3DApp{
private:
	std::shared_ptr<Shader> boxShader;
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Texture> boxTexture;

	GeometryGenerator geogem;
	

public:
	Sample5(HINSTANCE hInstance) : D3DApp(hInstance) {}
	~Sample5();
	void UpdateScene(float deltaTime) override;
	void OnStart() override;
	void Render() override;
};