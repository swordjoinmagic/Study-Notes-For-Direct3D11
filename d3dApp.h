#pragma once
#ifndef D3DAPP_H
#define D3DAPP_H

#include "D3DUtils.h"
#include "GameTimer.h"
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include "Camera.h"

class D3DApp {
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	// ������d3d����ľ��
	HINSTANCE AppInst() const;
	// ��ô��ھ��
	HWND MainWnd() const;
	// ��ô��ڱ���
	float AspectRatio() const;

	int Run();

	// ��ʼ�����ں�D3D������
	virtual bool Init();

	// �����ڸı�ʱ,�Զ����÷ֱ��ʸı��ģ��/��Ȼ�����
	virtual void OnResize();

	// ���ڸ��³����߼�(λ��/��ײ����)
	virtual void UpdateScene(float deltaTime = 0) {}

	// ��ʼ����d3dApp�ķ���,һ�����ڳ�ʼ��ģ��,����ṹ,��ɫ����
	virtual void OnStart() = 0;

	// ���Ƴ���(��������Ⱦ�ĺ���)
	virtual void Render() = 0;

	// ���ڹ��̺���,���ڴ��������Ϣ
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// ���ڴ�������¼�
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	// ������������¼�
	virtual void ProcessKeyBoardInput();
protected:
	// ��ʼ��Windows����
	bool InitMainWindow();
	// ��ʼ��D3D����
	bool InitDirect3D();
	void CalculateFrameState();
	virtual void DrawScene();

	// �������̨��Ϣ
	void ClearConsoleDebugInformation();
protected:
	// Ӧ�ó���ʵ�����
	HINSTANCE mhAppInst;
	// �����ھ��
	HWND mhMainWnd;	
	// �����Ƿ�����ͣ״̬
	bool mAppPaused;
	// �����Ƿ���С�����
	bool mMinimized;
	bool mMaximized;
	// �����Ƿ����ڸı��С��״̬
	bool mResizing;

	// 4x msaa�������ȼ�
	UINT m4xMessQuality;

	// ���ڼ�¼deltaTime�ļ�ʱ����
	GameTimer timer;

protected:
	// d3d�豸
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	int mClientWidth;
	int mClientHeight;

	bool mEnable4xMsaa;

	// ������
	std::wstring mMainWndCaption = L"Learning D3D";

protected:
	// ���������������ƶ�
	int lastX=0, lastY=0;
	bool firstMouse = true;
	std::shared_ptr<SJM::Camera> camera;

};

#endif // !D3DAPP_H
