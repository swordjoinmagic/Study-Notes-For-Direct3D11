#include "Sample13_ShadowMap.h"
#include <iostream>

void Sample13::OnStart() {
	// 初始化摄像机
	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 3, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 100;

	// 创建shadowMap
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = mClientWidth;
	depthDesc.Height = mClientHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	HR(md3dDevice->CreateTexture2D(&depthDesc,0,shadowMap.GetAddressOf()));

	// 创建用于测试的渲染深度时的屏幕纹理
	D3D11_TEXTURE2D_DESC offScreenDesc;
	offScreenDesc.Width = mClientWidth;
	offScreenDesc.Height = mClientHeight;
	offScreenDesc.MipLevels = 1;
	offScreenDesc.ArraySize = 1;
	offScreenDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	offScreenDesc.SampleDesc.Count = 1;
	offScreenDesc.SampleDesc.Quality = 0;
	offScreenDesc.Usage = D3D11_USAGE_DEFAULT;
	offScreenDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	offScreenDesc.CPUAccessFlags = 0;
	offScreenDesc.MiscFlags = 0;
	HR(md3dDevice->CreateTexture2D(&offScreenDesc,0,offScreenTex.GetAddressOf()));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(md3dDevice->CreateRenderTargetView(offScreenTex.Get(),&rtvDesc,offScreenTexRTV.GetAddressOf()));

	HR(md3dDevice->CreateShaderResourceView(offScreenTex.Get(),0,offScreenTexSRV.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;	
	// 创建深度视图
	HR(md3dDevice->CreateDepthStencilView(shadowMap.Get(),&dsvDesc,shadowMapDepthView.GetAddressOf()));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = depthDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	// 创建着色器资源视图
	HR(md3dDevice->CreateShaderResourceView(shadowMap.Get(),&srvDesc,shadowMapSRV.GetAddressOf()));

	// 初始化Shader
	shader = std::make_shared<Shader>(L"Shader/Sample12 ShadowMap/Compiled/simpleShadow.fxo",md3dDevice);
	createShadowMapShader = std::make_shared<Shader>(L"Shader/Sample12 ShadowMap/Compiled/CreateShadowMap.fxo",md3dDevice);
	postEffectShader = std::make_shared<Shader>(L"Shader/Sample9 ComputeShader/Compiled/postEffect.fxo", md3dDevice);


	// 初始化Mesh
	GeometryGenerator::MeshData meshData;
	geogen.CreateBox(1,1,1,meshData);
	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	boxMesh->CreateBuffer(md3dDevice);
	
	// 初始化纹理
	planeTexture = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");
	boxTexture = std::make_shared<Texture>(md3dDevice, L"Textures/Crate_Diffuse.jpg");

	// 初始化光源
	light.pos = float3(0,6,0);
	light.dir = float3(1,1,0);

	D3D11_INPUT_ELEMENT_DESC inputElementsDesc[] = {
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	// 初始化InputLayout
	md3dDevice->CreateInputLayout(&inputElementsDesc[0],4,shader->GetShaderInputSignature(0).pIAInputSignature,shader->GetShaderInputSignature(0).IAInputSignatureSize,shaderIL.GetAddressOf());
	md3dDevice->CreateInputLayout(&inputElementsDesc[0],4,createShadowMapShader->GetShaderInputSignature(0).pIAInputSignature,createShadowMapShader->GetShaderInputSignature(0).IAInputSignatureSize,createShadowMapShaderIL.GetAddressOf());


	// 初始化后处理渲染所需的Mesh
#pragma region 设置quad的顶点序列和索引序列
	std::vector<DefaultVertex> quadVertices(6);
	quadVertices[0] = (DefaultVertex(float3(-1, 1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(0, 1)));
	quadVertices[1] = (DefaultVertex(float3(1, -1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(1, 0)));
	quadVertices[2] = (DefaultVertex(float3(-1, -1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(0, 0)));
	quadVertices[3] = (DefaultVertex(float3(-1, 1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(0, 1)));
	quadVertices[4] = (DefaultVertex(float3(1, 1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(1, 1)));
	quadVertices[5] = (DefaultVertex(float3(1, -1, 0), float3(0, 0, 0), float3(0, 0, 0), float2(1, 0)));
	std::vector<uint> quadIndices(6);
	for (int i = 0; i < 6; i++) quadIndices[i] = i;
#pragma endregion

	quadMesh = std::make_shared<Mesh>(quadVertices, quadIndices);
	quadMesh->SetUp(postEffectShader->GetShaderInputSignature(0), md3dDevice);


	GeometryGenerator::MeshData sphereMeshData;
	geogen.CreateSphere(1,16,16,sphereMeshData);
	lightMesh = std::make_shared<Mesh>(sphereMeshData.vertices,sphereMeshData.indices);
	lightShader = std::make_shared<Shader>(L"Shader/Common/Compiled/DrawLightSphere.fxo", md3dDevice);

	lightMesh->SetUp(lightShader->GetShaderInputSignature(0),md3dDevice);
}

void Sample13::Render() {
	//// 第一步,设置只输出深度
	md3dImmediateContext->OMSetRenderTargets(1,offScreenTexRTV.GetAddressOf(),shadowMapDepthView.Get());
	// 清除颜色和深度缓冲
	md3dImmediateContext->ClearRenderTargetView(offScreenTexRTV.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(shadowMapDepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//// 第二步,渲染整个场景
	//XMVECTOR lightPos = XMVectorSet(9.31056,8.61791,- 11.7396,1.0);
	XMVECTOR lightPos = XMLoadFloat3(&light.pos);
	XMVECTOR lightDir = XMLoadFloat3(&light.dir);
	XMVECTOR focusPos = lightPos - lightDir;
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX view = XMMatrixLookAtLH(lightPos, focusPos, up);	
	XMMATRIX proj = XMMatrixOrthographicLH(5, 5, 0.1f, 100);	
	RenderSceneToCreateShadowMap(view,proj);
	
	
	//std::cout << camera->pos.x << " " << camera->pos.y << " " << camera->pos.z << std::endl;

	// 第三步,设置renderTarget为默认状态
	md3dImmediateContext->OMSetRenderTargets(1,&mRenderTargetView,mDepthStencilView);
	// 清除颜色和深度缓冲
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 渲染后处理纹理
	//RenderOffScreen();

	// 第四步,渲染场景
	XMMATRIX world2LightMatrix = view * proj;
	shader->SetMatrix4x4("World2LightMatrix", world2LightMatrix);	
	shader->SetShaderResource("shadowMap",shadowMapSRV.Get());
	shader->SetFloat4("shadowColor",float4(0.1,0.1,0.1,1));
	shader->SetFloat4("diffuseColor",float4(1,1,1,1));
	shader->SetFloat4("specularColor",float4(1,1,1,1));
	shader->SetRawValue("light", &light, sizeof(Light));
	shader->SetFloat3("viewPos", camera->pos);
	shader->SetFloat("bias",bias);


	RenderScene(view,proj,shader.get(),shaderIL.Get());
}

void Sample13::RenderSceneToCreateShadowMap(XMMATRIX view,XMMATRIX proj) {

	// 渲染平面
	XMMATRIX planeModel = XMMatrixScaling(20, 0.1, 20);
	XMMATRIX mvp = planeModel * view * proj;
	createShadowMapShader->SetMatrix4x4("mvp", mvp);
	createShadowMapShader->SetTexture2D("MainTex",*planeTexture);
	boxMesh->Draw(*createShadowMapShader, md3dImmediateContext, createShadowMapShaderIL.Get());

	// 渲染盒子
	XMMATRIX boxModel = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(MathF::Radians(30)) * XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(0, 3, 0);
	mvp = boxModel * view * proj;
	createShadowMapShader->SetMatrix4x4("mvp", mvp);
	createShadowMapShader->SetTexture2D("MainTex",*boxTexture);

	boxMesh->Draw(*createShadowMapShader, md3dImmediateContext, createShadowMapShaderIL.Get());

	XMMATRIX box2Model = XMMatrixScaling(1, 1, 1) *  XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(1, 0.5f, 0);
	mvp = box2Model * view * proj;

	createShadowMapShader->SetMatrix4x4("mvp", mvp);
	
	boxMesh->Draw(*createShadowMapShader, md3dImmediateContext, createShadowMapShaderIL.Get());
}

void Sample13::RenderScene(XMMATRIX LightView, XMMATRIX LightProj,Shader* shader,ID3D11InputLayout* inputLayout) {

	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();


	// 渲染光源
	XMMATRIX lightModel = XMMatrixTranslation(light.pos.x,light.pos.y,light.pos.z);
	XMMATRIX lightmvp = lightModel * view * proj;
	lightShader->SetMatrix4x4("mvp", lightmvp);
	lightMesh->Draw(*lightShader,md3dImmediateContext);


	// 渲染平面
	XMMATRIX planeModel = XMMatrixScaling(20, 0.1, 20);
	XMMATRIX mvp = planeModel * view * proj;
	XMMATRIX A = planeModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));


	shader->SetMatrix4x4("model", planeModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("transInvModel", transInvModel);
	shader->SetTexture2D("mainTex", *planeTexture);
	

	boxMesh->Draw(*shader, md3dImmediateContext,inputLayout);

	// 渲染盒子
	XMMATRIX boxModel = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(MathF::Radians(30)) * XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(0, 3, 0);
	mvp = boxModel * view * proj;
	A = boxModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));

	shader->SetMatrix4x4("model", boxModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("transInvModel", transInvModel);
	shader->SetTexture2D("mainTex", *boxTexture);

	boxMesh->Draw(*shader, md3dImmediateContext,inputLayout);

	XMMATRIX box2Model = XMMatrixScaling(1, 1, 1) *  XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(1, 0.5f, 0);
	mvp = box2Model * view * proj;
	A = box2Model;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));

	shader->SetMatrix4x4("model", box2Model);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("transInvModel", transInvModel);
	shader->SetTexture2D("mainTex", *boxTexture);

	boxMesh->Draw(*shader, md3dImmediateContext, inputLayout);
}

void Sample13::RenderOffScreen() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();

	XMMATRIX mvp = view * proj;

	postEffectShader->SetMatrix4x4("mvp",mvp);
	postEffectShader->SetShaderResource("MainTex", shadowMapSRV.Get());
	
	quadMesh->Draw(*postEffectShader,md3dImmediateContext);
}

void Sample13::UpdateScene(float deltaTime) {
	if (GetAsyncKeyState(VK_LEFT)<0) {
		XMVECTOR temp = XMLoadFloat3(&light.dir);
		XMMATRIX rotation = XMMatrixRotationY(-deltaTime*5);
		temp = XMVector3Transform(temp,rotation);
		XMStoreFloat3(&light.dir,temp);
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0) {
		XMVECTOR temp = XMLoadFloat3(&light.dir);
		XMMATRIX rotation = XMMatrixRotationY(deltaTime * 5);
		temp = XMVector3Transform(temp, rotation);
		XMStoreFloat3(&light.dir, temp);
	}
	if (GetAsyncKeyState(VK_KEY_Z)<0) {
		light.pos.x -= 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_KEY_X) < 0) {
		light.pos.x += 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_KEY_C) < 0) {
		light.pos.y -= 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_KEY_V) < 0) {
		light.pos.y += 5 * deltaTime;
	}
	if (GetAsyncKeyState(VK_KEY_Q) < 0) {
		bias += 0.01 * deltaTime;
	}
	if (GetAsyncKeyState(VK_KEY_E) < 0) {
		bias -= 0.01 * deltaTime;
	}
}