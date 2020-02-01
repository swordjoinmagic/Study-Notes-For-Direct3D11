#include "Sample12_SkyBox.h"

void Sample12::OnStart() {
	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 0, -5), float3(0, 0, 0), AspectRatio());
	

	// 初始化cubeMap
	//HR(D3DX11CreateShaderResourceViewFromFile(
	//	md3dDevice,
	//	L"Textures/testCube1.dds",
	//	0,0,
	//	cubeMapSRV.GetAddressOf(),
	//	0
	//));

	// 初始化Shader
	boxShader = std::make_shared<Shader>(L"Shader/Sample5 Texture/Compiled/textureMap.fxo",md3dDevice);
	shader = std::make_shared<Shader>(L"Shader/Sample11 CubeMap/Compiled/skyBox.fxo", md3dDevice);

	boxTexture = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");

	// 初始化CubeMap
	std::vector<std::wstring> paths(6);
	paths[0] = L"Textures/cubemap/posx.jpg";
	paths[1] = L"Textures/cubemap/negx.jpg";
	paths[2] = L"Textures/cubemap/posy.jpg";
	paths[3] = L"Textures/cubemap/negy.jpg";
	paths[4] = L"Textures/cubemap/posz.jpg";
	paths[5] = L"Textures/cubemap/negz.jpg";
	cubeMap = std::make_shared<CubeMap>(md3dDevice,md3dImmediateContext,paths);
	cubeMap->CreateShaderResourceView(md3dDevice,cubeMapSRV.GetAddressOf());

	// 初始化Mesh
	GeometryGenerator::MeshData meshData;
	geogen.CreateBox(1,1,1,meshData);
	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);

	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[] = {
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	md3dDevice->CreateInputLayout(&inputElementsDesc[0],4,boxShader->GetShaderInputSignature(0).pIAInputSignature,boxShader->GetShaderInputSignature(0).IAInputSignatureSize,boxIL.GetAddressOf());
	md3dDevice->CreateInputLayout(&inputElementsDesc[0], 4, shader->GetShaderInputSignature(0).pIAInputSignature, shader->GetShaderInputSignature(0).IAInputSignatureSize, skyIL.GetAddressOf());
	boxMesh->CreateBuffer(md3dDevice);
}
void Sample12::Render() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX vp = view * proj;
	boxShader->SetMatrix4x4("mvp",vp);
	boxShader->SetTexture2D("MainTex", *boxTexture);

	boxMesh->Draw(*boxShader,md3dImmediateContext,boxIL.Get());

	// 绘制天空盒
	shader->SetShaderResource("skyBox",cubeMapSRV.Get());	
	view.r[3] = XMVectorSet(0,0,0,1);
	vp = view * proj;
	shader->SetMatrix4x4("vp",vp);
	
	boxMesh->Draw(*shader,md3dImmediateContext,skyIL.Get());
}