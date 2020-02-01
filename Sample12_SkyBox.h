#pragma once

#include "d3dApp.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Texture.h"
#include "CubeMap.h"

class Sample12 : public D3DApp{
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> boxShader;
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<CubeMap> cubeMap;
	std::shared_ptr<Texture> boxTexture;

	ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	ComPtr<ID3D11InputLayout> boxIL;
	ComPtr<ID3D11InputLayout> skyIL;

	GeometryGenerator geogen;
public:
	Sample12(HINSTANCE hInstance):D3DApp(hInstance) {}
	virtual ~Sample12() {}

	void OnStart() override;
	void Render() override;
};