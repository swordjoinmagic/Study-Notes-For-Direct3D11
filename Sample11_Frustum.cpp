#include "Sample11_Frustum.h"

#include <iostream>

void Sample11::OnStart() {
	// ��ʼ�������
	camera = std::make_shared<SJM::Camera>(float3(0, 0, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 1000;

	data.resize(1000);
	// ��ʼ��ʵ������(�ܹ�1000��)
	// ���յ�10*10*10���ڷ�(���2.5��)
	for (int i = 0; i < 10;i++) {
		for (int j = 0; j < 10;j++) {
			for (int k = 0; k < 10;k++) {
				float3 pos(i*5,j*5,k*5);
				InstancedData id;
				id.model = XMMatrixTranslation(pos.x,pos.y,pos.z);
				id.model = XMMatrixTranspose(id.model);
				id.color = float4(i/10.0f,j/10.0f,k/10.f,1);

				data[i * 100 + j * 10 + k] = id;
				
			}
		}
	}

	// ��ʼ��Buffer
	HR(CreateBuffer(
		D3D11_USAGE_DYNAMIC,
		sizeof(InstancedData) * 1000,
		D3D11_BIND_SHADER_RESOURCE,
		D3D11_CPU_ACCESS_WRITE,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(InstancedData),
		0,
		md3dDevice,
		instancedDataBuffer.GetAddressOf()
	));
	
	// ��ʼ����ɫ����Դ
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = 1000;
	HR(md3dDevice->CreateShaderResourceView(instancedDataBuffer.Get(), &srvDesc, idSRV.GetAddressOf()));

	// ӳ������
	//D3D11_MAPPED_SUBRESOURCE mappedData;
	//md3dImmediateContext->Map(instancedDataBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData);

	//InstancedData* datas = reinterpret_cast<InstancedData*>(mappedData.pData);
	//for (int i = 0; i < 1000; i++) datas[i] = data[i];

	//md3dImmediateContext->Unmap(instancedDataBuffer.Get(),0);

	// ��ʼ��Shader
	shader = std::make_shared<Shader>(L"Shader/Sample10 Instancing/Compiled/frustum.fxo",md3dDevice);

	// ��ʼ������
	texture = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");

	// ��ʼ��Mesh
	GeometryGenerator::MeshData meshData;
	geogen.CreateSphere(1,16,16,meshData);

	mesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	mesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);


	// ��ʼ����׵�����ײ��Χ��(local Space)
	XNA::ComputeFrustumFromProjection(&frustumCollider,&camera->GetProjMatrix());
	// ��ʼ������İ�Χ��(local Space)
	XNA::ComputeBoundingSphereFromPoints(&sphereCollider,meshData.vertices.size(),&meshData.vertices[0].pos,sizeof(DefaultVertex));	

}

void Sample11::UpdateScene(float deltaTime) {
	instancedCount = 0;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	md3dImmediateContext->Map(instancedDataBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData);

	InstancedData* instanceBufferData = reinterpret_cast<InstancedData*>(mappedData.pData);

	// �۲�-����ռ�ı任����
	XMMATRIX invView = GetInverseMatrix(camera->GetViewMatrix());
	for (int i = 0; i < 1000;i++) {
		// ģ��-����ռ�ı任����
		XMMATRIX W = data[i].model;
		W = XMMatrixTranspose(W);
		// ����-ģ�Ϳռ�ı任����
		XMMATRIX invW = GetInverseMatrix(W);

		// �۲�ռ�-ģ�Ϳռ�ı任����
		XMMATRIX viewToLocal = XMMatrixMultiply(invView,invW);

		// �ֽ�viewToLocal����
		XMVECTOR scale;
		XMVECTOR rotQuat;
		XMVECTOR translation;
		XMMatrixDecompose(&scale,&rotQuat,&translation,viewToLocal);
				
		rotQuat = XMQuaternionNormalize(rotQuat);
		bool b = XMQuaternionIsNaN(rotQuat);
		

		// ����׵���Χ��任�������ģ�Ϳռ�
		XNA::Frustum localSpaceFrustum;
		XNA::TransformFrustum(&localSpaceFrustum,&frustumCollider,XMVectorGetX(scale),rotQuat,translation);
		
		// ���������ײ,�������ʵ�����ݼ��뻺��
		if (XNA::IntersectSphereFrustum(&sphereCollider,&localSpaceFrustum)!=0) {
			instanceBufferData[instancedCount++] = data[i];
		}
	}

	md3dImmediateContext->Unmap(instancedDataBuffer.Get(), 0);

	std::cout << instancedCount << std::endl;
}

void Sample11::Render() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjMatrix();
	shader->SetMatrix4x4("view",view);
	shader->SetMatrix4x4("proj",proj);
	shader->SetShaderResource("instancedDatas",idSRV.Get());
	shader->SetTexture2D("mainTex",*texture);

	mesh->DrawInstanced(*shader,md3dImmediateContext,1000);
}