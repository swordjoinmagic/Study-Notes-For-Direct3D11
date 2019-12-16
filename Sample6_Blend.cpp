#include "Sample6_Blend.h"

void Sample6::OnStart() {
	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());

	shader = std::make_shared<Shader>(L"Shader/Sample6 Blend/Compiled/simpleFog.fxo", md3dDevice);
	
	GeometryGenerator::MeshData meshData;
	geogem.CreateBox(1,1,1,meshData);

	boxMesh = std::make_shared<Mesh>(meshData.vertices, meshData.indices);
	boxMesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);

	gridTexture = std::make_shared<Texture>(md3dDevice, L"Textures/Grid.png");
	brickTexture = std::make_shared<Texture>(md3dDevice, L"Textures/blending_transparent_window.png");

	shader->SetFloat3("fogColor", float3(0.5f, 0.5f, 0.5f));
	shader->SetFloat("fogStart", 0);
	shader->SetFloat("fogRange", 10);

	for (int i = 0; i < 10;i++) {
		float randomX = 5 * (rand() / double(RAND_MAX));
		float randomY = 5 * (rand() / double(RAND_MAX));
		float randomZ = 5 * (rand() / double(RAND_MAX));
		randomTransform.push_back(float3(randomX,randomY,randomZ));
	}
}

void Sample6::UpdateScene(float deltaTime) {
	shader->SetFloat3("viewPos", camera->pos);

}

void Sample6::Render() {

	XMMATRIX m1 = XMMatrixTranslation(1, 0, 0);
	XMMATRIX m2 = XMMatrixTranslation(0.5f, 0, 2);
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();

	shader->SetTexture2D("MainTex", *brickTexture);

	for (float3 f : randomTransform) {
		XMMATRIX m = XMMatrixTranslation(f.x,f.y,f.z);
		XMMATRIX mvp = m * view*proj;
		shader->SetMatrix4x4("mvp", mvp);
		boxMesh->Draw(*shader, md3dImmediateContext);
	}
}