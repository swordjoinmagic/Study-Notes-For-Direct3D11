#pragma once
#include "d3dApp.h"
#include <vector>
#include "Shader.h"
struct Data {
	float3 v1;
	float2 v2;
};

class Sample9CSDemo : public D3DApp {
private:
	ID3D11Buffer* inputA;
	ID3D11Buffer* inputB;
	ID3D11Buffer* output;
	// 从显存复制到CPU的缓冲区
	ID3D11Buffer* outputDebugBuffer;

	ID3D11ShaderResourceView* inputAView;
	ID3D11ShaderResourceView* inputBView;
	ID3D11UnorderedAccessView* outputView;

	std::shared_ptr<Shader> cshader;
public:
	Sample9CSDemo(HINSTANCE hInstance) : D3DApp(hInstance) {}
	virtual ~Sample9CSDemo() {
		ReleaseCOM(inputA);
		ReleaseCOM(inputB);
		ReleaseCOM(output);
		ReleaseCOM(outputDebugBuffer);
		
		ReleaseCOM(inputAView);
		ReleaseCOM(inputBView);
		ReleaseCOM(outputView);
	}
	void OnStart() override;
	void DoComputeWork();
	void Render() override;
};