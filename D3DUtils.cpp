#include "stdafx.h"
#include "D3DUtils.h"
#include <D3Dcompiler.h>


ID3DBlob* LoadCompiledShaderFromFile(const std::wstring& fileName) {
	std::ifstream fin(fileName, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	ID3DBlob* blob;
	HR(D3DCreateBlob(size, &blob));
	
	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

std::vector<char> LoadCompiledEffectFile(const std::wstring& fileName,int& size) {
	std::ifstream fin(fileName,std::ios::binary);
	fin.seekg(0,std::ios_base::end);
	size = (int)fin.tellg();
	fin.seekg(0,std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0],size);
	fin.close();

	return compiledShader;
}