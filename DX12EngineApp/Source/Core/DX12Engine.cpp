#include "../stdafx.h"
#include "DX12Engine.h"

using namespace DirectX;


void DX12Engine::InitWindowSize(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
}

void DX12Engine::CreateDebugDevice()
{
	::CoInitialize(nullptr);	// 注意这里！DX12 的所有设备接口都是基于 COM 接口的，我们需要先全部初始化为 nullptr

#if defined(_DEBUG)		// 如果是 Debug 模式下编译，就执行下面的代码

	// 获取调试层设备接口
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12DebugDevice));
	// 开启调试层
	m_D3D12DebugDevice->EnableDebugLayer();
	// 开启调试层后，创建 DXGI 工厂也需要 Debug Flag
	m_DXGICreateFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;

#endif

}

bool DX12Engine::CreateDevice()
{
	CreateDXGIFactory2(m_DXGICreateFactoryFlag, IID_PPV_ARGS(& m_DXGIFactory));

	// DX12 支持的所有功能版本，你的显卡最低需要支持 11.0
	const D3D_FEATURE_LEVEL dx12SupportLevel[] =
	{
		D3D_FEATURE_LEVEL_12_2,		// 12.2
		D3D_FEATURE_LEVEL_12_1,		// 12.1
		D3D_FEATURE_LEVEL_12_0,		// 12.0
		D3D_FEATURE_LEVEL_11_1,		// 11.1
		D3D_FEATURE_LEVEL_11_0		// 11.0
	};

	// 用 EnumAdapters1 先遍历电脑上的每一块显卡
	// 每次调用 EnumAdapters1 找到显卡会自动创建 DXGIAdapter 接口，并返回 S_OK
	// 找不到显卡会返回 ERROR_NOT_FOUND
	ComPtr<IDXGIAdapter1> m_DXGIAdapter_tem;
	for (UINT i = 0; m_DXGIFactory->EnumAdapters1(i, m_DXGIAdapter_tem.GetAddressOf()) != ERROR_NOT_FOUND; i++)
	{
		//DXGI_ADAPTER_DESC desc;
		//if (SUCCEEDED(m_DXGIAdapter_tem->GetDesc(&desc)))
		//{
		//	std::wstring adapterName = desc.Description;
		//	std::wstring msg = L"FindGPU [" + std::to_wstring(i + 1) + L"]: " + adapterName;

		//	MessageBoxW(nullptr, msg.c_str(), L"CheckGPU", MB_OK | MB_ICONINFORMATION);
		//}

		m_DXGIAdapter_tem.As(&m_DXGIAdapter);
		// 找到显卡，就创建 D3D12 设备，从高到低遍历所有功能版本，创建成功就跳出
		for (const auto& level : dx12SupportLevel)
		{
			// 创建 D3D12 核心层设备，创建成功就返回 true
			if (SUCCEEDED(D3D12CreateDevice(m_DXGIAdapter.Get(), level, IID_PPV_ARGS(&m_D3D12Device))))
			{
				return true;
			}
		}
	}

	// 如果找不到任何能支持 DX12 的显卡，就退出程序
	if (m_D3D12Device == nullptr)
	{
		MessageBox(
			NULL,
			L"No DirectX 12-compatible graphics card was found. Please upgrade your system hardware.",
			L"Error",
			MB_OK | MB_ICONERROR
		);

		return false;
	}

}

void DX12Engine::CreateCommandComponents()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	// D3D12_COMMAND_LIST_TYPE_DIRECT 表示将命令都直接放进队列里，不做其他处理
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	m_D3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
	
	// 创建命令分配器，它的作用是开辟内存，存储命令列表上的命令，注意命令类型要一致
	m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

	m_D3D12Device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_CommandList)
	);

	m_CommandList->Close();
}

void DX12Engine::CreateRenderTarget(HWND hwnd)
{
	D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
	RTVHeapDesc.NumDescriptors = FrameCount;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_D3D12Device->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&m_RTVHeap));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = FrameCount;
	swapchainDesc.Width = WindowWidth;
	swapchainDesc.Height = WindowHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> temp_SwapChain;

	m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&temp_SwapChain
	);

	temp_SwapChain.As(&m_DXGISwapChain);

	RTVHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

	RTVDescriptionSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0;i < FrameCount;i++)
	{
		m_DXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTarget[i]));

		m_D3D12Device->CreateRenderTargetView(m_RenderTarget[i].Get(), nullptr, RTVHandle);

		RTVHandle.ptr += RTVDescriptionSize;
	}

}

void DX12Engine::CreateFenceAndBarrier()
{
	RenderEvent = CreateEvent(nullptr, false, true, nullptr);

	m_D3D12Device->CreateFence(0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)
	);



	beg_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	beg_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	beg_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	end_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	end_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	end_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
}

void DX12Engine::Render()
{
	RTVHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

	FrameIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	RTVHandle.ptr += FrameIndex * RTVDescriptionSize;

	m_CommandAllocator->Reset();

	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);

	beg_barrier.Transition.pResource = m_RenderTarget[FrameIndex].Get();

	m_CommandList->ResourceBarrier(1, &beg_barrier);

	// 用 RTV 句柄设置渲染目标
	m_CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);

	// 清空当前渲染目标的背景为天蓝色
	m_CommandList->ClearRenderTargetView(RTVHandle, DirectX::Colors::SkyBlue, 0, nullptr);

	end_barrier.Transition.pResource = m_RenderTarget[FrameIndex].Get();

	m_CommandList->ResourceBarrier(1, &end_barrier);

	m_CommandList->Close();

	// 用于传递命令用的临时 ID3D12CommandList 数组
	ID3D12CommandList* _temp_cmdlists[] = { m_CommandList.Get() };

	m_CommandQueue->ExecuteCommandLists(1, _temp_cmdlists);

	m_DXGISwapChain->Present(1, NULL);

	FenceValue++;

	m_CommandQueue->Signal(m_Fence.Get(), FenceValue);

	m_Fence->SetEventOnCompletion(FenceValue, RenderEvent);

	WaitForSingleObject(RenderEvent, INFINITE);

	
}


