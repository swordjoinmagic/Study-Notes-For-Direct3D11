#pragma once

#include "D3DUtils.h"

class Texture {
public:
	Texture(ID3D11Device* device,LPCWSTR filePath);
	~Texture();	
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;	
};