#pragma once
#include "GeometryGenerator.h"
#include "d3dApp.h"
#include "Light.h"
#include "Shader.h"


class Sample4 : public D3DApp{
private:

	struct Vertex {
		float3 pos;
		float3 normal;
	};

	ID3D11Buffer* objectIndexBuffer;
	ID3D11Buffer* objectVertexBuffer;

	std::shared_ptr<Shader> lightShader;
	std::shared_ptr<Shader> boxShader;
		
	GeometryGenerator geogen;

	ID3D11InputLayout* boxInputLayout;
	ID3D11InputLayout* lightInputLayout;

	Light parallelLight;
	
	GeometryGenerator::MeshData sphereMesh;

	uint indexCount;

	Light pointsLights;
	Light spotLight;

public:
	Sample4(HINSTANCE hInstance);
	~Sample4();
	void OnStart() override;
	void Render() override;
	void UpdateScene(float deltaTime) override;
	void ProcessKeyBoardInput() override;
};