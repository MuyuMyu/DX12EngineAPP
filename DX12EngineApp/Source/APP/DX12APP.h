#pragma once
#include <Windows.h>

using namespace Microsoft;
using namespace Microsoft::WRL;

class DX12APP
{
private:

	int WindowWidth = 640;		// 窗口宽度
	int WindowHeight = 480;		// 窗口高度
	HWND m_hwnd;				// 窗口句柄

public:

	HWND GetHWND()
	{
		return m_hwnd;
	}
	int GetWindowWidth() { return WindowWidth; }
	int GetWindowHeight() { return WindowHeight; }

	// 初始化窗口
	void InitWindow(HINSTANCE hins);

	// 渲染循环
	void RenderLoop();

	// 回调函数
	static LRESULT CALLBACK CallBackFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	static void Run(HINSTANCE hins)
	{
		DX12APP app;
		DX12Engine engine;
		app.InitWindow(hins);

		engine.InitWindowSize(app.GetWindowWidth(), app.GetWindowHeight());
		engine.CreateDebugDevice();
		engine.CreateDevice();
		engine.CreateCommandComponents();
		engine.CreateRenderTarget(app.GetHWND());

		app.RenderLoop();
	}
};

