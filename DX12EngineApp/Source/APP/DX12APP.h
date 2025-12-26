#pragma once
#include <Windows.h>

using namespace Microsoft;
using namespace Microsoft::WRL;

class DX12APP
{
private:

	int WindowWidth = 640;		// 窗口宽度
	int WindowHeight = 640;		// 窗口高度
	HWND m_hwnd;				// 窗口句柄
	DX12Engine engine;

public:

	HWND GetHWND() const
	{
		return m_hwnd;
	}
	int GetWindowWidth() const { return WindowWidth; }
	int GetWindowHeight() const { return WindowHeight; }

	// 初始化窗口
	void InitWindow(HINSTANCE hins);

	void InitDX12();

	// 渲染循环
	void RenderLoop();

	// 回调函数
	static LRESULT CALLBACK CallBackFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	static void Run(HINSTANCE hins)
	{
		DX12APP app;
		
		app.InitWindow(hins);
		app.InitDX12();
		app.RenderLoop();
	}
};

