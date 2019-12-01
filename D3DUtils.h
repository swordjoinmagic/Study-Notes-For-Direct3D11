#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <vector>
#include <d3dx11effect.h>
#include <string>
#include <sstream>
#include <fstream>

typedef XMFLOAT2 float2;
typedef XMFLOAT3 float3;
typedef XMFLOAT4 float4;
typedef XMFLOAT4X4 float4x4;
typedef XMCOLOR Color;
typedef UINT uint;


#ifndef HR
#define HR(x) {\
	\
	HRESULT hr = (x);\
	if(FAILED(hr)){\
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);\
	}\
}
#endif // !HR

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }


namespace Colors {
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}

ID3DBlob* LoadCompiledShaderFromFile(const std::wstring& fileName);
std::vector<char> LoadCompiledEffectFile(const std::wstring& fileName, int& size);