#pragma once

#include "d3dApp.h"
#include "GeometryGenerator.h"
#include "Mesh.h"

class Sample15 : public D3DApp {
private:
	struct Transform {
		float3 pos;
		float3 rotation;
		float3 scale;
	};
private:
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Shader> shader;
	
	std::shared_ptr<Texture> boxTexture;

	GeometryGenerator geogen;

	Transform parent;
	std::vector<Transform> childs;

	ID3D11InputLayout* inputLayout;

	float getRandData(int min, int max);
public:
	Sample15(HINSTANCE hInstance) : D3DApp(hInstance) {}
	~Sample15() {}
	void OnStart() override;
	void UpdateScene(float deltaTime) override;
	void Render() override;
};