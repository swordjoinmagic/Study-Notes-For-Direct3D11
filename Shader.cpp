#include "Shader.h"

Shader::Shader(const std::wstring &filePath, ID3D11Device* d3dDevice) {
	this->LoadCompiledShaderFromFile(filePath,d3dDevice);
}
Shader::~Shader() {
	ReleaseCOM(effect);	
}

void Shader::LoadCompiledShaderFromFile(const std::wstring &filePath,ID3D11Device* d3dDevice) {
	int size;
	std::vector<char> compiledShaderByteContent;
	compiledShaderByteContent = LoadCompiledEffectFile(filePath,size);
	D3DX11CreateEffectFromMemory(&compiledShaderByteContent[0],size,0,d3dDevice,&effect);

	technique = effect->GetTechniqueByIndex(0);
}

InputSignature Shader::GetShaderInputSignature(int index) {
	D3DX11_PASS_DESC passDesc;
	technique->GetPassByIndex(index)->GetDesc(&passDesc);
	
	InputSignature inputSignature;
	inputSignature.IAInputSignatureSize = passDesc.IAInputSignatureSize;
	inputSignature.pIAInputSignature = passDesc.pIAInputSignature;

	return inputSignature;
}

void Shader::SetFloat(const std::string &paramName, float value) {
	HR(effect->GetVariableByName(paramName.c_str())->SetRawValue(&value,0,sizeof(value)));
}
void Shader::SetFloat2(const std::string &paramName, const float2 &value) {
	XMVECTOR vector = XMLoadFloat2(&value);
	HR(effect->GetVariableByName(paramName.c_str())->AsVector()->SetFloatVector(reinterpret_cast<float*>(&vector)));
}
void Shader::SetFloat3(const std::string &paramName, const float3 &value) {
	XMVECTOR vector = XMLoadFloat3(&value);
	HR(effect->GetVariableByName(paramName.c_str())->AsVector()->SetFloatVector(reinterpret_cast<float*>(&vector)));
}
void Shader::SetFloat4(const std::string &paramName, const float4 &value) {
	XMVECTOR vector = XMLoadFloat4(&value);
	HR(effect->GetVariableByName(paramName.c_str())->AsVector()->SetFloatVector(reinterpret_cast<float*>(&vector)));
}
void Shader::SetMatrix4x4(const std::string& paramName, const float4x4 &value) {
	XMMATRIX marix = XMLoadFloat4x4(&value);
	HR(effect->GetVariableByName(paramName.c_str())->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&marix)));
}
void Shader::SetRawValue(const std::string& paramName, const void *pData, uint size) {
	HR(effect->GetVariableByName(paramName.c_str())->SetRawValue(pData,0,size));
}

void Shader::SetMatrix4x4(const std::string& paramName, XMMATRIX &value) {
	HR(effect->GetVariableByName(paramName.c_str())->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&value)));
}
void Shader::SetVector(const std::string& paramName, XMVECTOR &value) {
	HR(effect->GetVariableByName(paramName.c_str())->AsVector()->SetFloatVector(reinterpret_cast<float*>(&value)));
}
void Shader::SetTexture2D(const std::string& paramName,const Texture& texture) {
	HR(effect->GetVariableByName(paramName.c_str())->AsShaderResource()->SetResource(texture.shaderResourceView));	
}

void Shader::SetShaderResource(const std::string& paramName,ID3D11ShaderResourceView* value) {
	HR(effect->GetVariableByName(paramName.c_str())->AsShaderResource()->SetResource(value));
}
void Shader::SetUnorderedAccessView(const std::string& paramName, ID3D11UnorderedAccessView* value) {
	HR(effect->GetVariableByName(paramName.c_str())->AsUnorderedAccessView()->SetUnorderedAccessView(value));
}

void Shader::UsePass(int index, ID3D11DeviceContext* D3dDevicecontext) const{
	technique->GetPassByIndex(index)->Apply(0, D3dDevicecontext);
}

