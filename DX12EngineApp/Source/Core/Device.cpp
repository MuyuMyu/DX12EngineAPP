#include "Device.h"

Device::Device()
{
	::CoInitialize(nullptr);

#if defined(_DEBUG)		// 如果是 Debug 模式下编译，就执行下面的代码

	// 获取调试层设备接口
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugDevice));
	// 开启调试层
	m_DebugDevice->EnableDebugLayer();
	// 开启调试层后，创建 DXGI 工厂也需要 Debug Flag
	m_DXGICreateFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif

	CreateDXGIFactory2(m_DXGICreateFactoryFlag, IID_PPV_ARGS(&m_Factory));

	const D3D_FEATURE_LEVEL dx12SupportLevel[] =
	{
		D3D_FEATURE_LEVEL_12_2,		// 12.2
		D3D_FEATURE_LEVEL_12_1,		// 12.1
		D3D_FEATURE_LEVEL_12_0,		// 12.0
		D3D_FEATURE_LEVEL_11_1,		// 11.1
		D3D_FEATURE_LEVEL_11_0		// 11.0
	};

	Microsoft::WRL::ComPtr<IDXGIAdapter1> _tem_Adpater;
	for (UINT i = 0; m_Factory->EnumAdapters1(i, _tem_Adpater.GetAddressOf()) != ERROR_NOT_FOUND; i++)
	{
		_tem_Adpater.As(&m_Adapter);
		for (const auto& level : dx12SupportLevel)
		{
			if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), 
				level, 
				IID_PPV_ARGS(&m_Device))))
			{
				break;
			}
		}
	}

	if (m_Device == nullptr)
	{
		MessageBox(
			NULL,
			L"No DirectX 12-compatible graphics card was found. Please upgrade your system hardware.",
			L"Error",
			MB_OK | MB_ICONERROR
		);

		return;
	}

}
