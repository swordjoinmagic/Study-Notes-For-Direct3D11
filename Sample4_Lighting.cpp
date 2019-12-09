#include "Sample4_Lighting.h"
#include <cstdio>

Sample4::Sample4(HINSTANCE hInstance) : D3DApp(hInstance){}
Sample4::~Sample4() {}

void Sample4::OnStart() {

	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());
	camera->Far = 1000;

	// 初始化顶点与索引缓冲
	GeometryGenerator::MeshData mesh;
	//geogen.CreateBox(1, 1, 1, mesh);
	geogen.CreateSphere(1,32,32,mesh);
	std::vector<Vertex> vertices(mesh.vertices.size());
	indexCount = mesh.indices.size();
	for (uint i = 0; i < mesh.vertices.size(); i++) {
		vertices[i].pos = mesh.vertices[i].pos;
		vertices[i].normal = mesh.vertices[i].normal;
	}
	D3D11_BUFFER_DESC VBDesc;
	VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.ByteWidth = sizeof(Vertex)*vertices.size();
	VBDesc.MiscFlags = 0;
	VBDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = &vertices[0];

	md3dDevice->CreateBuffer(&VBDesc, &vbData, &objectVertexBuffer);

	D3D11_BUFFER_DESC IBDesc;
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.ByteWidth = sizeof(uint) * mesh.indices.size();
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = &mesh.indices[0];

	md3dDevice->CreateBuffer(&IBDesc, &ibData, &objectIndexBuffer);

	// 初始化Shader
	int size;
	std::vector<char> compiledShader = LoadCompiledEffectFile(L"Shader/Sample4 Lighting/Compiled/lightDemo.fxo",size);
	D3DX11CreateEffectFromMemory(&compiledShader[0],size,0,md3dDevice,&effect);
	technique = effect->GetTechniqueByIndex(0);

	// 初始化顶点输入布局
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	D3DX11_PASS_DESC pass;
	technique->GetPassByIndex(0)->GetDesc(&pass);
	md3dDevice->CreateInputLayout(&inputElements[0],2,pass.pIAInputSignature,pass.IAInputSignatureSize,&inputLayout);

	// 初始化光源
	parallelLight.dir = float3(1,1,0);
	parallelLight.lightColor = float3(1,1,1);
}


void Sample4::UpdateScene(float deltaTime) {

	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX mvp = model * view * proj;

	// 求mvp的逆转置矩阵	
	XMMATRIX A = model;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX TransInvMvp = XMMatrixTranspose(XMMatrixInverse(&det, A));

	XMVECTOR viewPos = XMLoadFloat3(&camera->pos);
	XMVECTOR lightColor = XMLoadFloat3(&parallelLight.lightColor);
	XMVECTOR lightDir = XMLoadFloat3(&parallelLight.dir);
	XMVECTOR diffuse = XMVectorSet(1, 1, 1,1);
	XMVECTOR specular = XMVectorSet(1, 1, 1,1);

	//printf("%f,%f,%f\n",parallelLight.dir.x, parallelLight.dir.y, parallelLight.dir.z);

	HR(effect->GetVariableByName("mvp")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&mvp)));
	HR(effect->GetVariableByName("transInvMvp")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&TransInvMvp)));
	HR(effect->GetVariableByName("model")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&model)));
	HR(effect->GetVariableByName("viewPos")->AsVector()->SetFloatVector(reinterpret_cast<float*>(&viewPos)));
	//HR(effect->GetVariableByName("color")->AsVector()->SetFloatVector(reinterpret_cast<float*>(&lightColor)));
	//HR(effect->GetVariableByName("dir")->AsVector()->SetFloatVector(reinterpret_cast<float*>(&lightDir)));
	effect->GetVariableByName("light")->SetRawValue(&parallelLight,0,sizeof(parallelLight));
	HR(effect->GetVariableByName("diffuse")->AsVector()->SetFloatVector(reinterpret_cast<float*>(&diffuse)));
	HR(effect->GetVariableByName("specular")->AsVector()->SetFloatVector(reinterpret_cast<float*>(&specular)));
	
}

void Sample4::Render() {
	// 设置填充模式
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 设置顶点输入布局
	md3dImmediateContext->IASetInputLayout(inputLayout);
	uint stride = sizeof(Vertex);
	uint offset = 0;
	// 设置顶点和索引缓冲
	md3dImmediateContext->IASetVertexBuffers(0,1,&objectVertexBuffer,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(objectIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	technique->GetPassByIndex(0)->Apply(0,md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(indexCount,0,0);
	

}