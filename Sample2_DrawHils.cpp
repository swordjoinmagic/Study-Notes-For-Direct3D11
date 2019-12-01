#include "stdafx.h"
#include "Sample2_DrawHils.h"

Sample2::Sample2(HINSTANCE hInstance):D3DApp(hInstance) {}
Sample2::~Sample2(){
	ReleaseCOM(hilsIB);
	ReleaseCOM(hilsVB);
	ReleaseCOM(fx);
	ReleaseCOM(inputLayout);
}

float Sample2::GetHeight(float x,float y) {
	return 0.3f * (y*sinf(0.1f*x) + x * cosf(0.1f*y));
}

void Sample2::OnStart() {

	// 利用网格生成器填充顶点数组
	GeometryGenerator::MeshData mesh;
	geogen.CreateGrid(160.0f, 160.0f, 50, 50,mesh);
	std::vector<Vertex> vertices(mesh.vertices.size());
	for (size_t i = 0; i < mesh.vertices.size();i++) {
		float3 p = mesh.vertices[i].pos;
		p.y = GetHeight(p.x,p.z);
		vertices[i].pos = p;
		if (p.y<-10.0f) {
			// beachColor
			vertices[i].color = float4(1.0f,0.96f,0.62f,1.0f);
		} else if(p.y<5.0f){
			// yellow-green
			vertices[i].color = float4(0.48f,0.77f,0.46f,1.0f);
		} else if(p.y<12.0f){
			// darkyellow-green
			vertices[i].color = float4(0.1f,0.48f,0.19f,1.0f);
		} else if(p.y<20.0f){
			vertices[i].color = float4(0.45f,0.39f,0.34f,1.0f);
		} else {
			// white
			vertices[i].color = float4(1,1,1,1);
		}
	}
	indexCount = mesh.indices.size();

	// 创建顶点缓冲和索引缓冲
	D3D11_BUFFER_DESC vbufferDesc;
	vbufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbufferDesc.ByteWidth = sizeof(Vertex)*mesh.vertices.size();
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbufferDesc,&vinitData,&hilsVB));

	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(uint)*mesh.indices.size();
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &mesh.indices[0];
	HR(md3dDevice->CreateBuffer(&iBufferDesc,&iinitData,&hilsIB));

	// 创建effect
	//ID3DBlob* shaderByteCode = LoadCompiledShaderFromFile(L"Shader\Sample1 DrawBox\test1.fxo");
	int size;
	std::vector<char> shaderByteCode = LoadCompiledEffectFile(L"Shader/Sample1 DrawBox/test1.fxo",size);
	//HR(D3DX11CreateEffectFromMemory(shaderByteCode->GetBufferPointer(),shaderByteCode->GetBufferSize(),0,md3dDevice,&fx));
	HR(D3DX11CreateEffectFromMemory(&shaderByteCode[0], size, 0, md3dDevice, &fx));
	//ReleaseCOM(shaderByteCode);

	tech = fx->GetTechniqueByIndex(0);
	fxmvp = fx->GetVariableByName("mvp")->AsMatrix();

	// 设置顶点输入布局
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	D3DX11_PASS_DESC passDesc;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc,2,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize,&inputLayout));
}

void Sample2::Render() {
	// 设置输入布局
	md3dImmediateContext->IASetInputLayout(inputLayout);
	// 设置填充模式
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 将顶点缓冲和缩影缓冲绑定至管线
	uint stride = sizeof(Vertex);
	uint offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0,1,&hilsVB,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(hilsIB,DXGI_FORMAT_R32_UINT,0);

	// 设置mvp矩阵
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0,15,-20,1.0f),XMVectorZero(),XMVectorSet(0,1.0f,0,0));
	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f*3.1415926f, AspectRatio(), 1.0f, 1000.0f);
	XMMATRIX mvp = model * view * proj;

	fxmvp->SetMatrix(reinterpret_cast<float*>(&mvp));

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (uint p = 0; p < techDesc.Passes; p++) {
		tech->GetPassByIndex(p)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(indexCount,0,0);
	}
}
