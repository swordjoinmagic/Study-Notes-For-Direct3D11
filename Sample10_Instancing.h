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

	// ʵ����Shader
	std::shared_ptr<Shader>  shader;
	
	std::shared_ptr<Mesh> boxMesh;	

	// ʵ�������������
	std::vector<InstanceData> instancedDatas;
	std::vector<MaterialData> materialDatas;

	// ʵ��������Buffer
	ComPtr<ID3D11Buffer> instancedDataBuffer;
	ComPtr<ID3D11Buffer> materialDataBuffer;

	// ʵ�������ݵ�SRV
	ComPtr<ID3D11ShaderResourceView> idSRV;
	ComPtr<ID3D11ShaderResourceView> mdSRV;
	
	// ��Դ
	Light light;
public:
	Sample10(HINSTANCE hInstance) :D3DApp(hInstance) {}
	virtual ~Sample10(){}
	void OnStart() override;
	void Render() override;

};