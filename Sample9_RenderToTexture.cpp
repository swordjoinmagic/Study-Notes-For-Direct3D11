#include "Sample9_RenderToTexture.h"

void Sample9RenderToTexture::InitTexView() {
	// 创建离屏纹理
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mClientWidth;
	texDesc.Height = mClientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	// 作为SRV,RT来使用
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	md3dDevice->CreateTexture2D(&texDesc, 0, &offScreenTex);

	// 创建离屏纹理的SRV
	md3dDevice->CreateShaderResourceView(offScreenTex, 0, &offScreenTexSRV);

	// 创建离屏纹理的RT
	md3dDevice->CreateRenderTargetView(offScreenTex, 0, &offScreenTexRT);
}

void Sample9RenderToTexture::OnStart() {

	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 3, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 100;

	// 初始化资源视图
	InitTexView();

	// 初始化场景物体
	GeometryGenerator::MeshData meshData;
	geogen.CreateBox(1, 1, 1, meshData);

	texShader = std::make_shared<Shader>(L"Shader/Sample7 DepthStencil/Compiled/diffuseShader.fxo", md3dDevice);

	boxMesh = std::make_shared<Mesh>(meshData.vertices, meshData.indices);
	boxMesh->SetUp(texShader->GetShaderInputSignature(0), md3dDevice);

	planeTex = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");
	boxTex = std::make_shared<Texture>(md3dDevice, L"Textures/Crate_Diffuse.jpg");

	light.dir = float3(1, 1, 0);
	light.lightColor = float3(1, 1, 1);

	texShader->SetRawValue("light", &light, sizeof(light));
	texShader->SetFloat3("diffuseColor", float3(1, 1, 1));
	texShader->SetFloat3("specularColor", float3(1, 1, 1));

	postEffectShader = std::make_shared<Shader>(L"Shader/Sample9 ComputeShader/Compiled/postEffect.fxo",md3dDevice);
	
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

	quadMesh = std::make_shared<Mesh>(quadVertices,quadIndices);
	quadMesh->SetUp(postEffectShader->GetShaderInputSignature(0),md3dDevice);

}

void Sample9RenderToTexture::DrawScene() {
	// 先将场景渲染到离屏纹理中

	ID3D11RenderTargetView* rtArrays[1] = {offScreenTexRT};

	// 第一步: 更改渲染管线的渲染目标为目标纹理
	md3dImmediateContext->OMSetRenderTargets(1,rtArrays,mDepthStencilView);
	// 清除纹理的颜色和深度缓冲区
	md3dImmediateContext->ClearRenderTargetView(offScreenTexRT,reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);

	// 绘制游戏场景
	DrawGameScene();

	// 第二步,更改渲染管线的渲染目标为后备缓冲区
	rtArrays[0] = mRenderTargetView;
	md3dImmediateContext->OMSetRenderTargets(1,rtArrays,mDepthStencilView);
	// 清除后备缓冲区中的颜色和深度缓冲
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制四边形(用于经过后处理后的游戏场景画面)
	DrawQuad();

	// 将后备缓冲区交换上来
	HR(mSwapChain->Present(0,0));
}

void Sample9RenderToTexture::DrawGameScene() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	texShader->SetFloat3("viewPos", camera->pos);

	// 渲染平面
	XMMATRIX planeModel = XMMatrixScaling(20, 0.1, 20);
	XMMATRIX mvp = planeModel * view * proj;
	XMMATRIX A = planeModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));

	texShader->SetMatrix4x4("model", planeModel);
	texShader->SetMatrix4x4("mvp", mvp);
	texShader->SetMatrix4x4("invModel", transInvModel);
	texShader->SetTexture2D("MainTex", *planeTex);

	boxMesh->Draw(*texShader, md3dImmediateContext);

	// 渲染盒子
	XMMATRIX boxModel = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(MathF::Radians(30)) * XMMatrixRotationY(MathF::Radians(30)) * XMMatrixTranslation(0, 3, 0);
	mvp = boxModel * view * proj;
	A = boxModel;
	A.r[3] = XMVectorSet(0, 0, 0, 1.0f);
	det = XMMatrixDeterminant(A);
	transInvModel = XMMatrixTranspose(XMMatrixInverse(&det, A));
	texShader->SetMatrix4x4("model", boxModel);
	texShader->SetMatrix4x4("mvp", mvp);
	texShader->SetMatrix4x4("invModel", transInvModel);
	texShader->SetTexture2D("MainTex", *boxTex);
	boxMesh->Draw(*texShader, md3dImmediateContext);
}


void Sample9RenderToTexture::DrawQuad() {
	postEffectShader->SetShaderResource("MainTex",offScreenTexSRV);
	quadMesh->Draw(*postEffectShader,md3dImmediateContext);
}

void Sample9RenderToTexture::Render() {}