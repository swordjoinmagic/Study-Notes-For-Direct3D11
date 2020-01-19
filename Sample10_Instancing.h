#pragma once

#include "d3dApp.h"
#include "Shader.h"
#include "Mesh.h"
#include "Light.h"
#include "GeometryGenerator.h"

class Sample10 : public D3DApp{
private:
	struct InstanceData {
		XMMATRIX model;
		XMMATRIX invModel;
		uint materialIndex;
	};
	struct MaterialData {
		float4 diffuseColor;
		float4 specularColor;
	};
private:
	GeometryGenerator geoGen;

	// 实例化Shader
	std::shared_ptr<Shader>  shader;
	
	std::shared_ptr<Mesh> boxMesh;	

	// 实例化所需的数据
	std::vector<InstanceData> instancedDatas;
	std::vector<MaterialData> materialDatas;

	// 实例化数据Buffer
	ComPtr<ID3D11Buffer> instancedDataBuffer;
	ComPtr<ID3D11Buffer> materialDataBuffer;

	// 实例化数据的SRV
	ComPtr<ID3D11ShaderResourceView> idSRV;
	ComPtr<ID3D11ShaderResourceView> mdSRV;
	
	// 光源
	Light light;
public:
	Sample10(HINSTANCE hInstance) :D3DApp(hInstance) {}
	virtual ~Sample10(){}
	void OnStart() override;
	void Render() override;

};