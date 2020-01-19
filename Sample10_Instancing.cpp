#include "Sample10_Instancing.h"

void Sample10::OnStart() {

	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 0, -5), float3(0, 0, 0), AspectRatio());

	// 初始化数据
	instancedDatas.resize(10);
	materialDatas.resize(10);

	// 排成10x10的矩阵形式的盒子,每个盒子间隔2.5米
	for (int i = 0; i < 10;i++) {
			float3 pos(2*i,0,0);			
			XMMATRIX model = XMMatrixTranslation(pos.x,pos.y,pos.z);			
			model = XMMatrixTranspose(model);

			// 去除平移
			XMMATRIX A = model;
			A.r[3] = XMVectorSet(0,0,0,1);
			// 求逆转置
			XMMATRIX transInvModel = XMMatrixTranspose(GetInverseMatrix(A));

			InstanceData id;
			//XMStoreFloat4x4(&id.model,model);
			//XMStoreFloat4x4(&id.invModel,transInvModel);
			id.model = model;
			id.invModel = transInvModel;
			id.materialIndex = i;

			instancedDatas[i] = id;	
	}
	for (int i = 0; i < 10; i++) {
		MaterialData md;
		md.diffuseColor = float4(float(i)/10.0f,1,0,1);
		md.specularColor = float4(1,1,1,1);
		materialDatas[i] = md;
	}

	// 初始化Buffer
	HR(CreateBuffer(
		D3D11_USAGE_DEFAULT,
		sizeof(InstanceData)*10,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(InstanceData),
		&instancedDatas[0],
		md3dDevice,
		instancedDataBuffer.GetAddressOf()
	));
	HR(CreateBuffer(
		D3D11_USAGE_DEFAULT,
		sizeof(MaterialData)*10,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(MaterialData),
		&materialDatas[0],
		md3dDevice,
		materialDataBuffer.GetAddressOf()
	));
	
	// 初始化着色器资源
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = 10;
	HR(md3dDevice->CreateShaderResourceView(instancedDataBuffer.Get(),&srvDesc,idSRV.GetAddressOf()));

	srvDesc.BufferEx.NumElements = 10;
	HR(md3dDevice->CreateShaderResourceView(materialDataBuffer.Get(),&srvDesc,mdSRV.GetAddressOf()));

	// 初始化Shader
	shader = std::make_shared<Shader>(L"Shader/Sample10 Instancing/Compiled/instancingShader.fxo",md3dDevice);

	// 初始化Mesh
	GeometryGenerator::MeshData boxMeshData;
	geoGen.CreateBox(1,1,1,boxMeshData);
	boxMesh = std::make_shared<Mesh>(boxMeshData.vertices,boxMeshData.indices);
	boxMesh->SetUp(shader->GetShaderInputSignature(0), md3dDevice);
	
	// 初始化光源
	light.dir = float3(1, 1, 0);
	light.lightColor = float3(1, 1, 1);
	
}

void Sample10::Render() {
	shader->SetRawValue("light",&light,sizeof(Light));
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	shader->SetMatrix4x4("view",view);
	shader->SetMatrix4x4("proj",proj);
	shader->SetShaderResource("gInstanceData",idSRV.Get());
	shader->SetShaderResource("gMaterialData",mdSRV.Get());

	boxMesh->DrawInstanced(*shader,md3dImmediateContext,10);
}