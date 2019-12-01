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
	// ��ʼ�����㻺��
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
	// ��ʼ����������
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
	// ��ʼ��Shader��������Ϊeffect�ļ�
	DWORD shaderFlags = 0;
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;

	// ���ڱ��������Shader�ֽ���
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

	// ����effect
	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),compiledShader->GetBufferSize(),0,md3dDevice,&mFX));
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByIndex(0);
	// ���shader��mvp�����ľ��
	mfxMVP = mFX->GetVariableByName("mvp")->AsMatrix();

	//==========================================
	// ���ö������벼���Լ������㻺����������������GPU
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		// ����	  ��������		�������Ը�ʽ		 �����	 ƫ����		    ����ʵ��������ͨ��Ⱦ  
		{"POSITION", 0  ,DXGI_FORMAT_R32G32B32_FLOAT,   0   , 0 ,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};


	// �����������벼��
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
	// �������벼��
	md3dImmediateContext->IASetInputLayout(mInputLayout);
	// ���û��Ƽ������filemode
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �����㻺������������������
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &boxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(boxIB, DXGI_FORMAT_R32_UINT, 0);

	// ����mvp����
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