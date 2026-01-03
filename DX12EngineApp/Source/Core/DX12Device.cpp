#include "DX12Device.h"
#include "../stdafx.h"

DX12Device::DX12Device()
{
	::CoInitialize(nullptr);

#if defined(_DEBUG)		// 如果是 Debug 模式下编译，就执行下面的代码

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugDevice))))
	{
		m_DebugDevice->EnableDebugLayer();
		// 可选：开启 GPU 验证（强烈推荐调试时打开）
		// m_DebugDevice->SetEnableGPUBasedValidation(TRUE);

		m_DXGICreateFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	// 检查 Factory 创建是否成功
	if (FAILED(CreateDXGIFactory2(m_DXGICreateFactoryFlag, IID_PPV_ARGS(&m_Factory))))
	{
		// Factory 创建失败，几乎不可能，但保险起见
		return;
	}

	const D3D_FEATURE_LEVEL dx12SupportLevel[] =
	{
		D3D_FEATURE_LEVEL_12_2,		// 12.2
		D3D_FEATURE_LEVEL_12_1,		// 12.1
		D3D_FEATURE_LEVEL_12_0,		// 12.0
		D3D_FEATURE_LEVEL_11_1,		// 11.1
		D3D_FEATURE_LEVEL_11_0		// 11.0
	};

	Microsoft::WRL::ComPtr<IDXGIAdapter1> _tem_Adpater;
	bool deviceCreated = false;

	for (UINT i = 0; m_Factory->EnumAdapters1(i, &_tem_Adpater) != ERROR_NOT_FOUND; i++)
	{
		
		for (const auto& level : dx12SupportLevel)
		{
			if (SUCCEEDED(D3D12CreateDevice(_tem_Adpater.Get(),
				level, 
				IID_PPV_ARGS(&m_Device))))
			{
				_tem_Adpater.As(&m_Adapter);
				deviceCreated = true;
				goto device_created;
			}
		}
	}

device_created:
	if (!deviceCreated)
	{
		MessageBox(
			NULL,
			L"No DirectX 12-compatible graphics card was found. Please upgrade your system hardware.",
			L"Error",
			MB_OK | MB_ICONERROR
		);
	}

	for (size_t i = 0; i < static_cast<size_t>(EQueueType::Count); i++)
	{
		auto type = static_cast<EQueueType>(i);
		m_CommandQueues[i] = std::make_unique<DX12CommandQueue>(m_Device.Get(),type);
	}
}

void DX12Device::Flush()
{
	for (auto& queue : m_CommandQueues)
	{
		queue->Flush();
	}
}

