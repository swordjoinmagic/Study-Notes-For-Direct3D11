#include "Sample4_Lighting.h"
#include <cstdio>

Sample4::Sample4(HINSTANCE hInstance) : D3DApp(hInstance){}
Sample4::~Sample4() {

}

void Sample4::OnStart() {

	// ��ʼ�������
	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());
	camera->Far = 1000;

	// ��ʼ����������������
	GeometryGenerator::MeshData mesh;
	geogen.CreateBox(1, 1, 1, mesh);

	box = std::make_shared<Mesh>(mesh.vertices,mesh.indices);

	geogen.CreateSphere(1, 32, 32, sphereMesh);
	lightSphere = std::make_shared<Mesh>(sphereMesh.vertices,sphereMesh.indices);
	
	// ��ʼ��Shader
	boxShader = std::make_shared<Shader>(L"Shader/Sample4 Lighting/Compiled/lightDemo.fxo",md3dDevice);
	lightShader = std::make_shared<Shader>(L"Shader/Common/Compiled/DrawLightSphere.fxo",md3dDevice);

	box->SetUp(boxShader->GetShaderInputSignature(0),md3dDevice);
	lightSphere->SetUp(lightShader->GetShaderInputSignature(0),md3dDevice);

	// ��ʼ����Դ
	parallelLight.dir = float3(1,1,0);
	parallelLight.lightColor = float3(1,1,1);
	
	// ��ʼ�����Դ
	pointsLights.lightColor = float3(1,1,1);
}


void Sample4::UpdateScene(float deltaTime) {

	XMMATRIX model = XMMatrixIdentity();	
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX mvp = model * view * proj;

	// ��model����ת�þ���	
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
	

	XMMATRIX lightModelMatrix = XMMatrixScaling(0.1f,0.1f,0.1f) * XMMatrixTranslation(pointsLights.pos.x,pointsLights.pos.y,pointsLights.pos.z);
	XMMATRIX lightMVP = lightModelMatrix * view * proj;
	lightShader->SetMatrix4x4("mvp",lightMVP);
}

void Sample4::Render() {
	
	box->Draw(*boxShader,md3dImmediateContext);

	lightSphere->Draw(*lightShader,md3dImmediateContext);
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