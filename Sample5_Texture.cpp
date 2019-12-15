#include "Sample5_Texture.h"

Sample5::~Sample5() {}

void Sample5::OnStart() {

	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());

	// ≥ı ºªØmesh
	GeometryGenerator::MeshData meshData;
	geogem.CreateBox(1,1,1,meshData);

	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	
	boxShader = std::make_shared<Shader>(L"Shader/Sample5 Texture/Compiled/textureMap.fxo", md3dDevice);

	boxMesh->SetUp(boxShader->GetShaderInputSignature(0),md3dDevice);

	boxTexture = std::make_shared<Texture>(md3dDevice,L"Textures/Brick_Diffuse.JPG");
}

void Sample5::UpdateScene(float deltaTime) {
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX mvp = model * view * proj;

	boxShader->SetMatrix4x4("mvp",mvp);
	boxShader->SetTexture2D("MainTex",*boxTexture);
}
void Sample5::Render() {
	boxMesh->Draw(*boxShader, md3dImmediateContext);
}