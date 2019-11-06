#include "stdafx.h"
#include "InitDirect3DApp.h"

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : D3DApp(hInstance){

}

InitDirect3DApp::~InitDirect3DApp() {	}

void InitDirect3DApp::OnResize() {
	D3DApp::OnResize();
}

void InitDirect3DApp::UpdateScene(float deltaTime) {

}

void InitDirect3DApp::DrawScene() {

	// ����Ϊ��ɫ
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView,reinterpret_cast<const float*>(&Colors::Blue));
	// �����Ⱥ�ģ�建��
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0,0);


	// ����˫����
	HR(mSwapChain->Present(0,0));
}