#pragma once

#include "d3dApp.h"

class Sample9RenderToTexture : public D3DApp{

public:
	Sample9RenderToTexture(HINSTANCE hInstance) : D3DApp(hInstance) {};
	~Sample9RenderToTexture();
	void OnStart() override;
	void Render() override;
};