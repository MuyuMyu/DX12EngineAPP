#include "../stdafx.h"
#include "DX12Engine.h"

using namespace DirectX;

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


