#include "Sample8_Billboard.h"

void Sample8::OnStart() {
	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());
	treeShader = std::make_shared<Shader>(L"Shader/Sample8 GeometryShader/Compiled/billboard.fxo",md3dDevice);
	treeTexture = std::make_shared<Texture>(md3dDevice,L"Textures/tree1.dds");

	// 初始化顶点索引和缓冲
	float3 pos(0,0,0);
	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.ByteWidth = sizeof(float3);
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = &pos;
	md3dDevice->CreateBuffer(&vbDesc,&vbData,&vertexBuffer);

	uint index = 0;
	D3D11_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = sizeof(uint);
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = &index;
	md3dDevice->CreateBuffer(&ibDesc,&ibData,&indexBuffer);


	D3D11_INPUT_ELEMENT_DESC ieDesc[] = {
		{"POSITION", 0  ,DXGI_FORMAT_R32G32B32_FLOAT,   0   , 0 ,	D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	InputSignature is = treeShader->GetShaderInputSignature(0);
	md3dDevice->CreateInputLayout(&ieDesc[0],1,is.pIAInputSignature,is.IAInputSignatureSize,&inputLayout);

}

void Sample8::UpdateScene(float deltaTime) {
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX vpMatrix = view * proj;
	
	treeShader->SetMatrix4x4("model",model);
	treeShader->SetMatrix4x4("vpMatrix",vpMatrix);
	treeShader->SetFloat2("Size",float2(10,10));
	treeShader->SetFloat3("viewPos",camera->pos);
	treeShader->SetTexture2D("MainTex",*treeTexture);
}

void Sample8::Render() {
	// 设置输入布局
	md3dImmediateContext->IASetInputLayout(inputLayout);
	// 设置绘制几何体的filemode
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	uint stride = sizeof(float3);
	uint offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R32_UINT,0);

	treeShader->UsePass(0,md3dImmediateContext);

	md3dImmediateContext->DrawIndexed(1,0,0);
}