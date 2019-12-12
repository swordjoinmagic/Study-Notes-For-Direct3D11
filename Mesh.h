#pragma once
#include "D3DUtils.h"
#include "Shader.h"
#include"DefaultVetexStruct.h"
class Mesh {
private:
	// ���㼯��
	std::vector<DefaultVertex> vertices;
	// ��������
	std::vector<uint> indices;

	// ������������
	ID3D11Buffer* verticesBuffer;
	ID3D11Buffer* indexBuffer;

	// �������벼��
	ID3D11InputLayout* inputLayout;
public:
	Mesh(const std::vector<DefaultVertex>& vertices, const std::vector<uint>& indices);
	~Mesh();
	void SetUp(const InputSignature& inputSignature,ID3D11Device* device);
	void SetUp(const InputSignature& inputSignature, ID3D11Device* device,D3D11_INPUT_ELEMENT_DESC* inputElementsDesc,uint size);
	void Draw(const Shader& shader,ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY primitiveTopology=D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;
};