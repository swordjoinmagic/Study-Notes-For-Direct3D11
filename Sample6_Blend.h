#pragma once
#include "d3dApp.h"
#include "D3DUtils.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "GeometryGenerator.h"

class Sample6 : public D3DApp{
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Texture> brickTexture;
	std::shared_ptr<Texture> gridTexture;

	GeometryGenerator geogem;

	ID3D11BlendState* blendState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilState2;

	std::vector<float3> randomTransform;
public:
	Sample6(HINSTANCE hInstance) : D3DApp(hInstance) {};
	~Sample6() {};
	void OnStart() override;
	void UpdateScene(float deltaTime) override;
	void Render() override;
};