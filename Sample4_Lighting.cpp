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
	geogen.CreateBox(1, 1, 1, mesh);
	geogen.CreateSphere(1,32,32,sphereMesh);
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
	boxShader = std::make_shared<Shader>(L"Shader/Sample4 Lighting/Compiled/lightDemo.fxo",md3dDevice);
	lightShader = std::make_shared<Shader>(L"Shader/Common/Compiled/DrawLightSphere.fxo",md3dDevice);

	// 初始化顶点输入布局
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}		
	};
	InputSignature boxIS = boxShader->GetShaderInputSignature(0);
	md3dDevice->CreateInputLayout(&inputElements[0],3, boxIS.pIAInputSignature, boxIS.IAInputSignatureSize,&boxInputLayout);

	D3D11_INPUT_ELEMENT_DESC lightInputElements[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	InputSignature lightIS = lightShader->GetShaderInputSignature(0);
	md3dDevice->CreateInputLayout(&lightInputElements[0],1,lightIS.pIAInputSignature,lightIS.IAInputSignatureSize,&lightInputLayout);

	// 初始化光源
	parallelLight.dir = float3(1,1,0);
	parallelLight.lightColor = float3(1,1,1);
	
	// 初始化点光源
	pointsLights.lightColor = float3(1,1,1);
}


void Sample4::UpdateScene(float deltaTime) {

	XMMATRIX model = XMMatrixIdentity();	
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX mvp = model * view * proj;

	// 求model的逆转置矩阵	
	XMMATRIX A = model;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));

	XMVECTOR viewPos = XMLoadFloat3(&camera->pos);
	XMVECTOR lightColor = XMLoadFloat3(&parallelLight.lightColor);
	XMVECTOR lightDir = XMLoadFloat3(&parallelLight.dir);
	XMVECTOR diffuse = XMVectorSet(1, 1, 1,1);
	XMVECTOR specular = XMVectorSet(1, 1, 1,1);

	boxShader->SetMatrix4x4("mvp",mvp);
	boxShader->SetMatrix4x4("transInvModel",transInvModel);
	boxShader->SetMatrix4x4("model",model);
	boxShader->SetFloat3("viewPos",camera->pos);
	boxShader->SetRawValue("light",&parallelLight,sizeof(parallelLight));
	boxShader->SetVector("diffuse",diffuse);
	boxShader->SetVector("specular",specular);
	
}

void Sample4::Render() {
	// 设置填充模式
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 设置顶点输入布局
	md3dImmediateContext->IASetInputLayout(boxInputLayout);
	uint stride = sizeof(Vertex);
	uint offset = 0;
	// 设置顶点和索引缓冲
	md3dImmediateContext->IASetVertexBuffers(0,1,&objectVertexBuffer,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(objectIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	boxShader->UsePass(0,md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(indexCount,0,0);
	

}

void Sample4::ProcessKeyBoardInput() {
	D3DApp::ProcessKeyBoardInput();
	if (GetAsyncKeyState(VK_UP)<0) 
		pointsLights.pos.y += 3*timer.DeltaTime();
	
	if (GetAsyncKeyState(VK_DOWN) < 0)
		pointsLights.pos.y -= 3*timer.DeltaTime();

	if (GetAsyncKeyState(VK_LEFT) < 0)
		pointsLights.pos.x -= 3*timer.DeltaTime();

	if (GetAsyncKeyState(VK_RIGHT) < 0)
		pointsLights.pos.x += 3*timer.DeltaTime();
}