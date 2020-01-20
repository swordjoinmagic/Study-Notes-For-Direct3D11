#include "Sample11_Frustum.h"

#include <iostream>

void Sample11::OnStart() {
	// 初始化摄像机
	camera = std::make_shared<SJM::Camera>(float3(0, 0, -5), float3(0, 0, 0), AspectRatio());
	camera->Far = 1000;

	data.resize(1000);
	// 初始化实例数据(总共1000个)
	// 按照的10*10*10规格摆放(间隔2.5米)
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

	// 初始化Buffer
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
	
	// 初始化着色器资源
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = 1000;
	HR(md3dDevice->CreateShaderResourceView(instancedDataBuffer.Get(), &srvDesc, idSRV.GetAddressOf()));

	// 映射数据
	//D3D11_MAPPED_SUBRESOURCE mappedData;
	//md3dImmediateContext->Map(instancedDataBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData);

	//InstancedData* datas = reinterpret_cast<InstancedData*>(mappedData.pData);
	//for (int i = 0; i < 1000; i++) datas[i] = data[i];

	//md3dImmediateContext->Unmap(instancedDataBuffer.Get(),0);

	// 初始化Shader
	shader = std::make_shared<Shader>(L"Shader/Sample10 Instancing/Compiled/frustum.fxo",md3dDevice);

	// 初始化纹理
	texture = std::make_shared<Texture>(md3dDevice, L"Textures/Brick_Diffuse.JPG");

	// 初始化Mesh
	GeometryGenerator::MeshData meshData;
	geogen.CreateSphere(1,16,16,meshData);

	mesh = std::make_shared<Mesh>(meshData.vertices,meshData.indices);
	mesh->SetUp(shader->GetShaderInputSignature(0),md3dDevice);


	// 初始化视椎体的碰撞包围体(local Space)
	XNA::ComputeFrustumFromProjection(&frustumCollider,&camera->GetProjMatrix());
	// 初始化球体的包围球(local Space)
	XNA::ComputeBoundingSphereFromPoints(&sphereCollider,meshData.vertices.size(),&meshData.vertices[0].pos,sizeof(DefaultVertex));	

}

void Sample11::UpdateScene(float deltaTime) {
	instancedCount = 0;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	md3dImmediateContext->Map(instancedDataBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData);

	InstancedData* instanceBufferData = reinterpret_cast<InstancedData*>(mappedData.pData);

	// 观察-世界空间的变换矩阵
	XMMATRIX invView = GetInverseMatrix(camera->GetViewMatrix());
	for (int i = 0; i < 1000;i++) {
		// 模型-世界空间的变换矩阵
		XMMATRIX W = data[i].model;
		W = XMMatrixTranspose(W);
		// 世界-模型空间的变换矩阵
		XMMATRIX invW = GetInverseMatrix(W);

		// 观察空间-模型空间的变换矩阵
		XMMATRIX viewToLocal = XMMatrixMultiply(invView,invW);

		// 分解viewToLocal矩阵
		XMVECTOR scale;
		XMVECTOR rotQuat;
		XMVECTOR translation;
		XMMatrixDecompose(&scale,&rotQuat,&translation,viewToLocal);
				
		rotQuat = XMQuaternionNormalize(rotQuat);
		bool b = XMQuaternionIsNaN(rotQuat);
		

		// 将视椎体包围体变换到物体的模型空间
		XNA::Frustum localSpaceFrustum;
		XNA::TransformFrustum(&localSpaceFrustum,&frustumCollider,XMVectorGetX(scale),rotQuat,translation);
		
		// 如果发生碰撞,把物体的实例数据加入缓冲
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