#include "../stdafx.h"
#include "DX12APP.h"

void DX12APP::InitWindow(HINSTANCE hins)
{;

	WNDCLASS wc = {};
	wc.hInstance = hins;
	wc.lpfnWndProc = CallBackFunc;
	wc.lpszClassName = L"DX12MainWindow";
	wc.hbrBackground = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClass(&wc);

	m_hwnd = CreateWindow(wc.lpszClassName,
		L"DX12Wind",
		WS_SYSMENU | WS_OVERLAPPED,
		0,
		0,
		WindowWidth,
		WindowHeight,
		NULL,
		NULL,
		hins,
		NULL
	);

	// 因为指定了窗口大小不可变的 WS_SYSMENU 和 WS_OVERLAPPED，应用不会自动显示窗口，需要使用 ShowWindow 强制显示窗口
	ShowWindow(m_hwnd, SW_SHOW);
}

void DX12APP::InitDX12()
{
	engine.InitWindowSize(WindowWidth, WindowHeight);
	if (!engine.Initialize(m_hwnd))
		return;
	engine.CreateCommandComponents();
	engine.CreateRenderTarget(m_hwnd);
	engine.CreateFenceAndBarrier();
	engine.CreatePipeline();

}

void DX12APP::RenderLoop()
{
	bool isExited = false;
	MSG msg = {};

	while (!isExited)
	{
		HANDLE RenderEvent = engine.GetRenderEvent();

		DWORD ActiveEvent = MsgWaitForMultipleObjects(1,
			&RenderEvent,
			false,
			INFINITE,
			QS_ALLINPUT);

		switch (ActiveEvent - WAIT_OBJECT_0)
		{
		case 0:					// ActiveEvent 是 0，说明渲染事件已经完成了，进行下一次渲染
			engine.Render();
			break;
			
		case 1:					// ActiveEvent 是 1，说明渲染事件未完成，CPU 主线程同时处理窗口消息，防止界面假死
			// 查看消息队列是否有消息，如果有就获取。 PM_REMOVE 表示获取完消息，就立刻将该消息从消息队列中移除
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message != WM_QUIT)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					isExited = true;
				}
			}
			break;

		case WAIT_TIMEOUT:
			break;
		}
	}

}

LRESULT DX12APP::CallBackFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);

	}

	return 0;
}
