#include "Mesh.h"

Mesh::Mesh(const std::vector<DefaultVertex>& vertices, const std::vector<uint>& indices) {
	this->vertices = vertices;
	this->indices = indices;
}
Mesh::~Mesh() {
	ReleaseCOM(verticesBuffer);
	ReleaseCOM(indexBuffer);
	ReleaseCOM(inputLayout);
}

void Mesh::CreateBuffer(ID3D11Device* device) {
	// 创建顶点/索引缓冲
	D3D11_BUFFER_DESC vbufferDesc;
	vbufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbufferDesc.ByteWidth = sizeof(DefaultVertex)*vertices.size();
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbufferDesc, &vinitData, &verticesBuffer));

	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(uint)*indices.size();
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&iBufferDesc, &iinitData, &indexBuffer));
}

void Mesh::SetUp(const InputSignature& inputSignature, ID3D11Device* device) {
	// 创建顶点/索引缓冲
	D3D11_BUFFER_DESC vbufferDesc;
	vbufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbufferDesc.ByteWidth = sizeof(DefaultVertex)*vertices.size();
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbufferDesc, &vinitData, &verticesBuffer));

	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(uint)*indices.size();
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&iBufferDesc, &iinitData, &indexBuffer));


	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	device->CreateInputLayout(&inputElementsDesc[0],4,inputSignature.pIAInputSignature,inputSignature.IAInputSignatureSize,&inputLayout);
}
void Mesh::SetUp(const InputSignature& inputSignature, ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* inputElementsDesc,uint size) {
	device->CreateInputLayout(inputElementsDesc, size, inputSignature.pIAInputSignature, inputSignature.IAInputSignatureSize, &inputLayout);
}

void Mesh::Draw(const Shader& shader, ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY primitiveTopology) const {
	this->Draw(shader,deviceContext,this->inputLayout);
}

void Mesh::Draw(const Shader& shader, ID3D11DeviceContext* deviceContext,ID3D11InputLayout* inputLayout, D3D_PRIMITIVE_TOPOLOGY primitiveTopology) const {
	// 设置绘制模式
	deviceContext->IASetPrimitiveTopology(primitiveTopology);
	// 设置顶点输入布局
	deviceContext->IASetInputLayout(inputLayout);
	uint stride = sizeof(DefaultVertex);
	uint offset = 0;
	// 设置顶点和索引缓冲
	deviceContext->IASetVertexBuffers(0, 1, &verticesBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	shader.UsePass(0,deviceContext);

	deviceContext->DrawIndexed(indices.size(), 0, 0);

}

void Mesh::DrawInstanced(const Shader& shader, ID3D11DeviceContext* deviceContext, uint count, D3D_PRIMITIVE_TOPOLOGY primitiveTopology) {
	// 设置绘制模式
	deviceContext->IASetPrimitiveTopology(primitiveTopology);
	// 设置顶点输入布局
	deviceContext->IASetInputLayout(inputLayout);
	uint stride = sizeof(DefaultVertex);
	uint offset = 0;
	// 设置顶点和索引缓冲
	deviceContext->IASetVertexBuffers(0, 1, &verticesBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	shader.UsePass(0, deviceContext);

	deviceContext->DrawIndexedInstanced(indices.size(),count,0,0,0);
}
