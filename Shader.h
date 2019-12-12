#pragma once

#include "D3DUtils.h"
#include "InputSignature.h"


class Shader{
private:
	ID3DX11Effect* effect;
	ID3DX11EffectTechnique* technique;
public:
	Shader();
	Shader(const std::wstring &filePath, ID3D11Device* d3dDevice);
	~Shader();

	void UsePass(int index, ID3D11DeviceContext* D3dDevicecontext) const;

	// 从文件中读取shader
	void LoadCompiledShaderFromFile(const std::wstring &filePath, ID3D11Device* d3dDevice);

	// 获得下标为index的pass的顶点着色器输入参数的字节码(主要用于dx创建顶点输入布局)
	InputSignature GetShaderInputSignature(int index);

	void SetMatrix4x4(const std::string& paramName, const float4x4 &value);
	void SetFloat4(const std::string& paramName, const float4 &value);
	void SetFloat3(const std::string& paramName, const float3 &value);
	void SetFloat2(const std::string& paramName, const float2 &value);
	void SetFloat(const std::string& paramName, float value);
	void SetRawValue(const std::string& paramName,const void *pData,uint size);
	void SetMatrix4x4(const std::string& paramName, XMMATRIX &value);
	void SetVector(const std::string& paramName, XMVECTOR &value);
};