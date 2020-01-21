#pragma once
#include "D3DUtils.h"
#include "Shader.h"
#include"DefaultVetexStruct.h"
class Mesh {
private:
	// 顶点集合
	std::vector<DefaultVertex> vertices;
	// 索引集合
	std::vector<uint> indices;

	// 顶点索引缓冲
	ID3D11Buffer* verticesBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	// 顶点输入布局
	ID3D11InputLayout* inputLayout = nullptr;
public:
	Mesh(const std::vector<DefaultVertex>& vertices, const std::vector<uint>& indices);
	~Mesh();
	void CreateBuffer(ID3D11Device* device);
	void SetUp(const InputSignature& inputSignature,ID3D11Device* device);
	void SetUp(const InputSignature& inputSignature, ID3D11Device* device,D3D11_INPUT_ELEMENT_DESC* inputElementsDesc,uint size);
	void Draw(const Shader& shader, ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;
	void Draw(const Shader& shader,ID3D11DeviceContext* deviceContext,ID3D11InputLayout* inputlayout, D3D_PRIMITIVE_TOPOLOGY primitiveTopology=D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;
	void DrawInstanced(const Shader& shader, ID3D11DeviceContext* deviceContext,uint count, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};