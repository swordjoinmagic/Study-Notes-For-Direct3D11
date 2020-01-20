#pragma once

#include "d3dApp.h"
#include "Mesh.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Texture.h"
#include "xnacollision.h"

class Sample11 : public D3DApp{
private:
	struct InstancedData {
		XMMATRIX model;
		float4 color;
	};
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Texture> texture;

	GeometryGenerator geogen;

	std::vector<InstancedData> data;
	ComPtr<ID3D11Buffer> instancedDataBuffer;
	ComPtr<ID3D11ShaderResourceView> idSRV;

	XNA::Frustum frustumCollider;
	XNA::Sphere sphereCollider;

	uint instancedCount;
public:
	Sample11(HINSTANCE hInstance):D3DApp(hInstance) {}
	~Sample11() {}
	void OnStart() override;
	void UpdateScene(float deltaTime) override;
	void Render() override;
};