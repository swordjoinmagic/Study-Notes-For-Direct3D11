#pragma once

#include "D3DUtils.h"


// 简化CubeMap生成类
class CubeMap {
private:
	ComPtr<ID3D11Texture2D> textureCube;
public:	
	CubeMap(ID3D11Device* device, 
		ID3D11DeviceContext* context, 
		std::vector<std::wstring>& paths);
	void CreateShaderResourceView(ID3D11Device* device,ID3D11ShaderResourceView** result);
	virtual ~CubeMap();
};