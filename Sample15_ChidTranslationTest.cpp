
#include "Sample15_ChidTranslationTest.h"

void Sample15::OnStart() {
	

	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 3, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 100;

	shader = std::make_shared<Shader>(L"Shader/Sample5 Texture/Compiled/textureMap.fxo", md3dDevice);

	GeometryGenerator::MeshData meshData;
	geogen.CreateBox(1,1,1,meshData);
	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	//boxMesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);
	boxMesh->CreateBuffer(md3dDevice);

	// 初始化父物体
	parent.pos = float3(0,0,0);
	parent.rotation = float3(0,0,0);
	parent.scale = float3(1,1,1);

	// 初始化10个子物体,pos约束在(-3,-3,-3)~(3,3,3),rotation约束在0~360,scale约束在0.5~1.5
	childs.resize(10);
	for (int i = 0; i < 10;i++) {		
		childs[i].pos = float3(getRandData(-3, 3), getRandData(-3, 3), getRandData(-3, 3));
		childs[i].rotation = float3(getRandData(0,360), getRandData(0, 360), getRandData(0, 360));
		childs[i].scale = float3(getRandData(0.5,1.5), getRandData(0.5, 1.5), getRandData(0.5, 1.5));
		
	}

	boxTexture = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");

	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[] = {
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},	
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	md3dDevice->CreateInputLayout(&inputElementsDesc[0],2,shader->GetShaderInputSignature(0).pIAInputSignature,shader->GetShaderInputSignature(0).IAInputSignatureSize,&inputLayout);
}

void Sample15::UpdateScene(float deltaTime) {
	if (GetAsyncKeyState(VK_UP) < 0) {
		parent.pos.z += 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0) {
		parent.pos.z -= 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_LEFT) < 0) {
		parent.rotation.y -= 2 * deltaTime;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0) {
		parent.rotation.y += 2 * deltaTime;
	}
}

void Sample15::Render() {

	

	shader->SetTexture2D("MainTex",*boxTexture);
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();

	XMMATRIX parentScaleMatrix = XMMatrixScaling(parent.scale.x, parent.scale.y, parent.scale.z);
	XMMATRIX parentRotationMatrix = XMMatrixRotationY(parent.rotation.y) * XMMatrixRotationX(parent.rotation.x) * XMMatrixRotationZ(parent.rotation.z);
	XMMATRIX parentTranslationMatrix = XMMatrixTranslation(parent.pos.x, parent.pos.y, parent.pos.z);

	XMMATRIX parentModel = parentScaleMatrix * parentRotationMatrix * parentTranslationMatrix;

	XMMATRIX mvp = parentModel * view * proj;
	shader->SetMatrix4x4("mvp",mvp);

	boxMesh->Draw(*shader,md3dImmediateContext,inputLayout);

	for (int i = 0; i < 10;i++) {
		XMMATRIX ScaleMatrix =  XMMatrixScaling(0.5,0.5,0.5);
		XMMATRIX RotationMatrix = XMMatrixRotationY(childs[i].rotation.y) * XMMatrixRotationX(childs[i].rotation.x) * XMMatrixRotationZ(childs[i].rotation.z);
		XMMATRIX TranslationMatrix = XMMatrixTranslation(childs[i].pos.x, childs[i].pos.y, childs[i].pos.z);

		XMMATRIX Model = ScaleMatrix * RotationMatrix * TranslationMatrix * parentModel;

		mvp = Model * view * proj;
		
		shader->SetMatrix4x4("mvp",mvp);

		boxMesh->Draw(*shader,md3dImmediateContext, inputLayout);
	}
}


float Sample15::getRandData(int min, int max) {
	float m1 = (float)(rand() % 101) / 101;
	min++;
	float m2 = (float)((rand() % (max - min + 1)) + min);
	m2 = m2 - 1;
	return m1 + m2;
}
