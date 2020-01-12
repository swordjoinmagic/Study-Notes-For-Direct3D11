#include "Sample9_SimpleCSDemo_VecAddcpp.h"
#include <iostream>
void Sample9CSDemo::OnStart() {
	int number = 5;
	std::vector<Data> A(number);
	std::vector<Data> B(number);
	for (int i = 0; i < number;i++) {
		A[i].v1 = float3(i,i,i);
		A[i].v2 = float2(i,0);
		B[i].v1 = float3(-i,i,0);
		B[i].v2 = float2(0,-i);
	}

	// 初始化缓冲
	HR(CreateBuffer(
		D3D11_USAGE_DEFAULT,
		sizeof(Data)*number,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(Data),
		&A[0],
		md3dDevice,
		&inputA));

	HR(CreateBuffer(
		D3D11_USAGE_DEFAULT,
		sizeof(Data)*number,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(Data),
		&B[0],
		md3dDevice,
		&inputB));

	HR(CreateBuffer(		
		D3D11_USAGE_DEFAULT,
		sizeof(Data)*number,
		D3D11_BIND_UNORDERED_ACCESS,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(Data),
		0,
		md3dDevice,
		&output
	));
	HR(CreateBuffer(
		D3D11_USAGE_STAGING,
		sizeof(Data) * number,
		0,
		D3D11_CPU_ACCESS_READ,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(Data),
		0,
		md3dDevice,
		&outputDebugBuffer
	));

	// 创建自定义数据结构AB的着色器视图
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = number;

	HR(md3dDevice->CreateShaderResourceView(inputA,&srvDesc,&inputAView));
	HR(md3dDevice->CreateShaderResourceView(inputB, &srvDesc, &inputBView));

	// 创建输出数据的output的uav视图
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = number;
	
	HR(md3dDevice->CreateUnorderedAccessView(output,&uavDesc,&outputView));



	// 初始化计算着色器
	cshader = std::make_shared<Shader>(L"Shader/Sample9 ComputeShader/Compiled/VecAdd.fxo",md3dDevice);

	DoComputeWork();
}

void Sample9CSDemo::DoComputeWork() {
	// 输入参数
	cshader->SetShaderResource("gInputA", inputAView);
	cshader->SetShaderResource("gInputB", inputBView);
	cshader->SetUnorderedAccessView("gOutput", outputView);

	cshader->UsePass(0,md3dImmediateContext);
	// 启动线程组(计算着色器)
	md3dImmediateContext->Dispatch(1,1,1);

	// 解绑srv和uav,用来节省性能
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	ID3D11UnorderedAccessView* nullUAV[1] = {0};
	md3dImmediateContext->CSSetShaderResources(0,1,nullSRV);
	md3dImmediateContext->CSSetUnorderedAccessViews(0,1,nullUAV,0);

	// 将计算着色器从管线中解绑
	md3dImmediateContext->CSSetShader(0,0,0);

	// 复制输出的缓冲区到系统内存
	md3dImmediateContext->CopyResource(outputDebugBuffer,output);

	// 将在显存里的缓冲区映射到outputDebug这个缓冲区上
	D3D11_MAPPED_SUBRESOURCE mappedData;
	md3dImmediateContext->Map(outputDebugBuffer,0,D3D11_MAP_READ,0,&mappedData);

	Data* dataView = reinterpret_cast<Data*>(mappedData.pData);

	for (int i = 0; i < 5;i++) {
		std::cout << 
			"v1("<<
			dataView[i].v1.x <<
			","<<
			dataView[i].v1.y<<
			","<<
			dataView[i].v1.z<<
			")"
		<<std::endl;
	}

	md3dImmediateContext->Unmap(outputDebugBuffer,0);
}

void Sample9CSDemo::Render() {}