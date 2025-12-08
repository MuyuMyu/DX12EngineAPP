#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;
class DX12Engine
{
private:
	//调试层
	ComPtr<ID3D12Debug6> m_D3D12DebugDevice;
	UINT m_DXGICreateFactoryFlag = NULL;

	//设备层
	ComPtr<IDXGIFactory7> m_DXGIFactory;
	ComPtr<IDXGIAdapter4> m_DXGIAdapter;
	ComPtr<ID3D12Device14> m_D3D12Device;

	//命令组件
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> m_CommandList;

public:

	void CreateDebugDevice();

	bool CreateDevice();

	void CreateCommandComponents();
};

