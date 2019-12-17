#include "Sample7_DepthStencil.h"

#include <iostream>

void Sample7::OnStart() {
	camera = std::make_shared<SJM::Camera>(float3(0,3,-5),float3(0,0,0),AspectRatio());
	camera->Far = 100;

	GeometryGenerator::MeshData meshData;
	geoGem.CreateBox(1,1,1,meshData);

	shader = std::make_shared<Shader>(L"Shader/Sample7 DepthStencil/Compiled/diffuseShader.fxo",md3dDevice);

	boxMesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	boxMesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);
	
	mirrorTex = std::make_shared<Texture>(md3dDevice,L"Textures/blending_transparent_window.png");
	planeTex = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");
	boxTex = std::make_shared<Texture>(md3dDevice, L"Textures/Crate_Diffuse.jpg");

	light.dir = float3(1,1,0);
	light.lightColor = float3(1,1,1);

	shader->SetRawValue("light",&light,sizeof(light));
	shader->SetFloat3("diffuseColor",float3(1,1,1));
	shader->SetFloat3("specularColor",float3(1,1,1));

	// 初始化混合状态
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC renderBlendDesc;
	renderBlendDesc.BlendEnable = true;
	renderBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;	// rgb混合中的Fsrc混合因子
	renderBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// rgb混合中的Fdest混合因子
	renderBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;	// alpha混合中的Fsrc混合因子
	renderBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;	// alpha混合中的Fdest混合因子
	renderBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0] = renderBlendDesc;
	HR(md3dDevice->CreateBlendState(&blendDesc, &blendState));

	// 初始化深度/模板状态
	D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	
	// 不写入深度 && 模板测试在深度测试通过时通过,通过后会将相应模板缓冲区改为1
	depthStateDesc.DepthEnable = true;	// 开启深度测试
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// 关闭深度写入
	depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStateDesc.StencilEnable = true;
	depthStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStateDesc.BackFace = depthStateDesc.FrontFace;
	
	md3dDevice->CreateDepthStencilState(&depthStateDesc,&doNotWriteDepth);
	
	// 开启深度测试&&深度写入&&模板测试,当模板缓冲区值不等于模板参考值时,不绘制像素
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	md3dDevice->CreateDepthStencilState(&depthStateDesc,&stencilTest);


	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	md3dDevice->CreateBlendState(&blendDesc,&doNotRenderTarget);
}

void Sample7::UpdateScene(float deltaTime) {

}

void Sample7::Render() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	shader->SetFloat3("viewPos",camera->pos);
	

	// 渲染平面
	XMMATRIX planeModel = XMMatrixScaling(20,0.1,20);
	XMMATRIX mvp = planeModel * view * proj;
	XMMATRIX A = planeModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));

	shader->SetMatrix4x4("model",planeModel);
	shader->SetMatrix4x4("mvp",mvp);
	shader->SetMatrix4x4("invModel",transInvModel);
	shader->SetTexture2D("MainTex",*planeTex);

	boxMesh->Draw(*shader,md3dImmediateContext);

	// 渲染盒子
	XMMATRIX boxModel = XMMatrixScaling(1,1,1) * XMMatrixRotationX(MathF::Radians(30)) * XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(boxPos.x,boxPos.y,boxPos.z);
	mvp = boxModel * view * proj;
	A = boxModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));
	shader->SetMatrix4x4("model", boxModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("invModel", transInvModel);
	shader->SetTexture2D("MainTex", *boxTex);
	boxMesh->Draw(*shader, md3dImmediateContext);

	float a[4] = {0,0,0,0};
	// 开启混合
	md3dImmediateContext->OMSetBlendState(doNotRenderTarget, a, 0xffffffff);
	// 开启模板测试关闭深度写入
	md3dImmediateContext->OMSetDepthStencilState(doNotWriteDepth,1);	
	// 渲染镜子到模板缓冲区
	XMMATRIX mirroModel = XMMatrixScaling(10,10,0.1)*XMMatrixTranslation(0,5,3);
	mvp = mirroModel * view * proj;
	A = mirroModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));
	shader->SetMatrix4x4("model", mirroModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("invModel", transInvModel);
	shader->SetTexture2D("MainTex", *mirrorTex);	
	boxMesh->Draw(*shader, md3dImmediateContext);
	
	//// reset
	md3dImmediateContext->OMSetBlendState(0, nullptr, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);	
	

	// 绘制box的镜像
	// 开启模板测试
	md3dImmediateContext->OMSetDepthStencilState(stencilTest, 1);
	XMVECTOR mirrorPlane = XMVectorSet(0,0,1,0);
	XMMATRIX R = XMMatrixReflect(mirrorPlane);
	boxModel = boxModel * R * XMMatrixTranslation(0,0,6);
	mvp = boxModel * view * proj;
	A = boxModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));
	shader->SetMatrix4x4("model", boxModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("invModel", transInvModel);
	shader->SetTexture2D("MainTex", *boxTex);
	boxMesh->Draw(*shader, md3dImmediateContext);


	md3dImmediateContext->OMSetDepthStencilState(0,0);
	// 正常的绘制镜子
	// 开启混合
	md3dImmediateContext->OMSetBlendState(blendState, a, 0xff);
	mvp = mirroModel * view * proj;
	A = mirroModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));
	shader->SetMatrix4x4("model", mirroModel);
	shader->SetMatrix4x4("mvp", mvp);
	shader->SetMatrix4x4("invModel", transInvModel);
	shader->SetTexture2D("MainTex", *mirrorTex);
	boxMesh->Draw(*shader, md3dImmediateContext);
	
	// reset
	md3dImmediateContext->OMSetBlendState(0, nullptr, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
}

void Sample7::ProcessKeyBoardInput() {
	D3DApp::ProcessKeyBoardInput();

	if (GetAsyncKeyState(VK_LEFT)<0) {
		boxPos.x -= 5 * timer.DeltaTime();
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0) {
		boxPos.x += 5 * timer.DeltaTime();
	}
}