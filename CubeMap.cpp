#include "CubeMap.h"


CubeMap::~CubeMap() {}

/*
	从六张天空盒的正方形贴图转换成CubeMap,
	读入的六张贴图按照如下顺序
		D3D11_TEXTURECUBE_FACE_POSITIVE_X = 0,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_X = 1,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Y = 2,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Y = 3,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Z = 4,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Z = 5

	步骤:
		1. 读取这六张正方形贴图
		2. 创建包含6个纹理的纹理数组
		3. 将六张贴图完整地拷贝到该数组中
		4. 创建该李繁体纹理的SRV
*/
CubeMap::CubeMap(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::vector<std::wstring>& paths) {
	std::vector<ID3D11Texture2D*> texArray(6,nullptr);
	std::vector<D3D11_TEXTURE2D_DESC> texDescArray(6);
		
	for (uint i = 0; i < 6;i++) {
		HRESULT hr;		
		D3DX11CreateTextureFromFile(
			device,
			paths[i].c_str(),
			0,0,
			(ID3D11Resource**)&texArray[i],
			&hr
		);		
		if (FAILED(hr)) {			
			DXTrace(__FILE__, (DWORD)__LINE__, hr, 0, true); 
		}
		texArray[i]->GetDesc(&texDescArray[i]);
	}	

	// 填充纹理描述
	D3D11_TEXTURE2D_DESC cubeMapDesc;
	cubeMapDesc.Width = texDescArray[0].Width;
	cubeMapDesc.Height = texDescArray[0].Height;
	cubeMapDesc.MipLevels = 1;	// mipmap默认级别为1
	cubeMapDesc.ArraySize = 6;
	cubeMapDesc.Format = texDescArray[0].Format;
	cubeMapDesc.SampleDesc.Count = 1;
	cubeMapDesc.SampleDesc.Quality = 0;
	cubeMapDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cubeMapDesc.CPUAccessFlags = 0;
	cubeMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// 创建CubeMap纹理对象
	device->CreateTexture2D(&cubeMapDesc,nullptr,textureCube.GetAddressOf());


	// 将六个纹理对象逐一赋值过去
	for (uint i = 0; i < 6;i++) {
		// 将六个纹理对象的mipmap子资源逐一赋值过去
		for (uint j = 0; j < cubeMapDesc.MipLevels;j++) {
			context->CopySubresourceRegion(
				textureCube.Get(),
				D3D11CalcSubresource(j,i,cubeMapDesc.MipLevels),
				0,0,0,
				texArray[i],
				j,
				nullptr
			);
		}
	}
}

void CubeMap::CreateShaderResourceView(ID3D11Device* device, ID3D11ShaderResourceView** result) {
	D3D11_TEXTURE2D_DESC texDesc;
	textureCube.Get()->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(textureCube.Get(),&srvDesc,result));	
}