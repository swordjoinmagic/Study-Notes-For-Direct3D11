#include "stdafx.h"
#include "Sample1_DrawBox.h"

float angle = 0;

Sample1::Sample1(HINSTANCE hInstance) : 
	D3DApp(hInstance){
	mMainWndCaption = L"box Demo";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mModel,I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}
Sample1::~Sample1() {	
	ReleaseCOM(boxIB);
	ReleaseCOM(boxVB);	
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);	
}


void Sample1::OnStart() {

	//==========================================
	// 初始化顶点缓冲
	Vertex vertices[] = {
		{float3(-1,-1,-1),(const float*)&Colors::White },
		{float3(-1,1,-1),(const float*)&Colors::Black },
		{float3(1,1,-1),(const float*)&Colors::Red },
		{float3(1,-1,-1),(const float*)&Colors::Green },
		{float3(-1,-1,1),(const float*)&Colors::Blue },
		{float3(-1,1,1),(const float*)&Colors::Yellow },
		{float3(1,1,1),(const float*)&Colors::Cyan },
		{float3(1,-1,1),(const float*)&Colors::Magenta },
	};
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vertexBufferDesc,&vinitData,&boxVB));

	//==========================================
	// 初始化索引缓冲
	UINT indices[] = {
		0,1,2,
		0,2,3,

		4,6,5,
		4,7,6,

		4,5,1,
		4,1,0,

		3,2,6,
		3,6,7,

		1,5,6,
		1,6,2,

		4,0,3,
		4,3,7
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&indexBufferDesc,&indexInitData,&boxIB));

	//==========================================
	// 初始化Shader，并编译为effect文件
	DWORD shaderFlags = 0;
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;

	// 用于保存编译后的Shader字节码
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(
		L"Shader/Sample1 DrawBox/test1.fx",
		0,0,0,
		"fx_5_0",
		shaderFlags,0,0,
		&compiledShader,&compilationMsgs,
		0);
	if (compilationMsgs != 0) {
		MessageBoxA(0,(char*)compilationMsgs->GetBufferPointer(),0,0);
		ReleaseCOM(compilationMsgs);
	}

	if (FAILED(hr)) {
		DXTrace(__FILE__,(DWORD)__LINE__,hr,L"D3DX11CompileFrameFile",true);
	}

	// 创建effect
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),compiledShader->GetBufferSize(),0,md3dDevice,&mFX));
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByIndex(0);
	// 获得shader中mvp变量的句柄
	mfxMVP = mFX->GetVariableByName("mvp")->AsMatrix();

	//==========================================
	// 设置顶点输入布局以及将顶点缓冲和索引缓冲输入进GPU
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		// 语义	  语义索引		顶点属性格式		 输入槽	 偏移量		    区分实例化和普通渲染  
		{"POSITION", 0  ,DXGI_FORMAT_R32G32B32_FLOAT,   0   , 0 ,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};


	// 创建顶点输入布局
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc,2,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize,&mInputLayout));
	
}

void Sample1::UpdateScene(float deltaTime) {

	angle += deltaTime;

	XMVECTOR pos = XMVectorSet(0,0,-5,1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0,1.0f,0,0);

	XMMATRIX V = XMMatrixLookAtLH(pos,target,up);
	XMStoreFloat4x4(&mView,V);

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*3.1415926f,AspectRatio(),1.0f,10.0f);
	XMStoreFloat4x4(&mProj,P);

	XMMATRIX M = XMMatrixRotationX(angle)*XMMatrixRotationY(angle)*XMMatrixRotationZ(angle);
	XMStoreFloat4x4(&mModel,M);
}

void Sample1::Render() {
	// 设置输入布局
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	// 设置绘制几何体的filemode
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 将顶点缓冲和索引缓冲绑定至管线
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &boxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(boxIB, DXGI_FORMAT_R32_UINT, 0);

	// 设置mvp矩阵
	XMMATRIX model = XMLoadFloat4x4(&mModel);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX mvpMatrix = model * view * proj;

	mfxMVP->SetMatrix(reinterpret_cast<float*>(&mvpMatrix));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (uint p = 0; p < techDesc.Passes; p++) {
		mTech->GetPassByIndex(0)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(36,0,0);
	}

	
}