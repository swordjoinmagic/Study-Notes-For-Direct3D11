#include "Sample3_DrawShape.h"
#include "GeometryGenerator.h"

Sample3::Sample3(HINSTANCE hInstance):D3DApp(hInstance) {}
Sample3::~Sample3() {
	ReleaseCOM(effect);
	ReleaseCOM(shapeIndexBuffer);
	ReleaseCOM(shapeVertexBuffer);
	ReleaseCOM(inputLayout);
	
}



void Sample3::OnStart() {
	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0,0,-5),float3(0,0,0),AspectRatio());
	camera->Far = 1000;

	// 初始化顶点/索引缓冲区
	GeometryGenerator::MeshData mesh;
	geoGen.CreateCylinder(3,5,5,256,4,mesh);
	indexCount = mesh.indices.size();

	std::vector<Vertex> vertices(mesh.vertices.size());
	for (uint i = 0; i < mesh.vertices.size(); i++) vertices[i].pos = mesh.vertices[i].pos;

	D3D11_BUFFER_DESC shapeVertexBufferDesc;
	shapeVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	shapeVertexBufferDesc.ByteWidth = sizeof(Vertex)*vertices.size();
	shapeVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	shapeVertexBufferDesc.MiscFlags = 0;
	shapeVertexBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &vertices[0];

	md3dDevice->CreateBuffer(&shapeVertexBufferDesc,&vertexData,&shapeVertexBuffer);

	D3D11_BUFFER_DESC shapeIndexBufferDesc;
	shapeIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	shapeIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	shapeIndexBufferDesc.ByteWidth = sizeof(uint)*mesh.indices.size();
	shapeIndexBufferDesc.CPUAccessFlags = 0;
	shapeIndexBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &mesh.indices[0];

	md3dDevice->CreateBuffer(&shapeIndexBufferDesc,&indexData,&shapeIndexBuffer);

	// 初始化shader
	int size;
	std::vector<char> compiledEffectFile = LoadCompiledEffectFile(L"Shader/Sample3 DrawShape/Compiled/drawCylinder.fxo",size);
	D3DX11CreateEffectFromMemory(&compiledEffectFile[0],size,0,md3dDevice,&effect);
	technique = effect->GetTechniqueByIndex(0);

	// 初始化输入布局
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {		
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	D3DX11_PASS_DESC passDesc;
	technique->GetPassByIndex(0)->GetDesc(&passDesc);
	md3dDevice->CreateInputLayout(vertexDesc,1,passDesc.pIAInputSignature,passDesc.IAInputSignatureSize,&inputLayout);
}

void Sample3::Render() {
	// 设置输入布局
	md3dImmediateContext->IASetInputLayout(inputLayout);
	// 设置填充模式
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 设置顶点/索引缓冲
	uint stride = sizeof(Vertex);
	uint offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0,1,&shapeVertexBuffer,&stride,&offset);
	md3dImmediateContext->IASetIndexBuffer(shapeIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	// 设置mvp矩阵
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	XMMATRIX mvp = model * view * proj;
	effect->GetVariableByName("model")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&model));
	effect->GetVariableByName("view")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&view));
	effect->GetVariableByName("proj")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&proj));

	D3DX11_TECHNIQUE_DESC techniqueDesc;
	HR(technique->GetDesc(&techniqueDesc));
	for (uint i = 0; i < techniqueDesc.Passes;i++) {
		technique->GetPassByIndex(0)->Apply(0,md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(indexCount,0,0);
	}
}