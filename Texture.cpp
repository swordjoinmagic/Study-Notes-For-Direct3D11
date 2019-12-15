#include "Texture.h"

Texture::Texture(ID3D11Device* device, LPCWSTR filePath) {
	HR(D3DX11CreateShaderResourceViewFromFile(device, filePath, nullptr, nullptr, &shaderResourceView, nullptr));
}
Texture::~Texture() {
	ReleaseCOM(shaderResourceView);
	ReleaseCOM(renderTargetView);	
}