#include "Sample14_Projector.h"

void Sample14::OnStart() {

	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 3, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 100;

	// 初始化Shader
	shader = std::make_shared<Shader>(L"Shader/Sample13 Projector/Compiled/simpleProjector.fxo",md3dDevice);

	projTex = std::make_shared<Texture>(md3dDevice, L"Textures/7c1ed21b0ef41bd56e07d84e5cda81cb39db3d37.jpg");
	mainTex = std::make_shared<Texture>(md3dDevice, L"Textures/Crate_Diffuse.jpg");


	// 初始化Mesh
	GeometryGenerator::MeshData meshData;
	geogen.CreateBox(1,1,1,meshData);
	
	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	boxMesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);

	shader->SetTexture2D("MainTex",*mainTex);
	shader->SetTexture2D("projTex",*projTex);

	// 初始化投影器方向
	projectorPos = float3(0,0,-3);
}

void Sample14::Render() {
	
	// 初始化投影器的VP矩阵
	XMVECTOR projPos = XMLoadFloat3(&projectorPos);
	XMVECTOR projDir = XMVectorSet(0,0,1,0);
	XMMATRIX projView = XMMatrixLookAtLH(projPos,projPos+projDir,XMVectorSet(0,1,0,0));
	XMMATRIX projProj = XMMatrixPerspectiveFovLH(90,AspectRatio(),0.1,100);
	XMMATRIX projVP = projView * projProj;
	shader->SetMatrix4x4("projectorVP",projVP);

	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();

	// 第一个箱子的坐标
	XMMATRIX box1Model = XMMatrixTranslation(-1,0,0);

	XMMATRIX mvp = box1Model * view * proj;
	shader->SetMatrix4x4("mvp",mvp);
	shader->SetMatrix4x4("model",box1Model);

	boxMesh->Draw(*shader,md3dImmediateContext);

	// 第二个箱子的坐标
	XMMATRIX box2Model = XMMatrixTranslation(1,0,0);
	mvp = box2Model * view * proj;

	shader->SetMatrix4x4("mvp",mvp);
	shader->SetMatrix4x4("model",box2Model);

	boxMesh->Draw(*shader,md3dImmediateContext);
	
}

void Sample14::UpdateScene(float deltaTime) {
	if (GetAsyncKeyState(VK_LEFT) < 0) {
		projectorPos.x -= 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_RIGHT)<0) {
		projectorPos.x += 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_UP) < 0) {
		projectorPos.y -= 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0) {
		projectorPos.y += 5 * deltaTime;
	}
}