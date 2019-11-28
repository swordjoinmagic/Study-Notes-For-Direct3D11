// LearnDirect3D11.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "LearnDirect3D11.h"
#include "D3DUtils.h"
#include "InitDirect3DApp.h"
#include "Sample1_DrawBox.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{	
	Sample1 theApp(hInstance);
	
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


