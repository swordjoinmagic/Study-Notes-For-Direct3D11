#include "Sample10_Instancing.h"

void Sample10::OnStart() {

	// ��ʼ�������
	camera = std::make_shared<SJM::Camera>(float3(0, 0, -5), float3(0, 0, 0), AspectRatio());

	// ��ʼ������
	instancedDatas.resize(10);
	materialDatas.resize(10);

	// �ų�10x10�ľ�����ʽ�ĺ���,ÿ�����Ӽ��2.5��
	for (int i = 0; i < 10;i++) {
			float3 pos(2*i,0,0);			
			XMMATRIX model = XMMatrixTranslation(pos.x,pos.y,pos.z);			
			model = XMMatrixTranspose(model);

			// ȥ��ƽ��
			XMMATRIX A = model;
			A.r[3] = XMVectorSet(0,0,0,1);
			// ����ת��
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

	// ��ʼ��Buffer
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
	
	// ��ʼ����ɫ����Դ
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = 10;
	HR(md3dDevice->CreateShaderResourceView(instancedDataBuffer.Get(),&srvDesc,idSRV.GetAddressOf()));

	srvDesc.BufferEx.NumElements = 10;
	HR(md3dDevice->CreateShaderResourceView(materialDataBuffer.Get(),&srvDesc,mdSRV.GetAddressOf()));

	// ��ʼ��Shader
	shader = std::make_shared<Shader>(L"Shader/Sample10 Instancing/Compiled/instancingShader.fxo",md3dDevice);

	// ��ʼ��Mesh
	GeometryGenerator::MeshData boxMeshData;
	geoGen.CreateBox(1,1,1,boxMeshData);
	boxMesh = std::make_shared<Mesh>(boxMeshData.vertices,boxMeshData.indices);
	boxMesh->SetUp(shader->GetShaderInputSignature(0), md3dDevice);
	
	// ��ʼ����Դ
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