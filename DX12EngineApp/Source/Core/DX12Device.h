#pragma once
#include "DX12CommandQueue.h"
#include <dxgi1_6.h>
#include <memory>
#include <array>

class DX12Device
{

private:
	Microsoft::WRL::ComPtr<ID3D12Debug6> m_DebugDevice;
	UINT m_DXGICreateFactoryFlag = NULL;
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
	Microsoft::WRL::ComPtr<ID3D12Device14> m_Device;

	std::array<std::unique_ptr<DX12CommandQueue>, static_cast<size_t>(EQueueType::Count)> m_CommandQueues;

public:

	DX12Device();
	~DX12Device() = default;

	IDXGIFactory7* GetFactory()  { return m_Factory.Get(); }
	IDXGIAdapter4* GetAdapter()  { return m_Adapter.Get(); }
	ID3D12Device14* GetDevice()  { return m_Device.Get(); }
	DX12CommandQueue* GetCommandQueue(EQueueType type) { return m_CommandQueues[static_cast<size_t>(type)].get(); }
	bool IsInitialized() { return m_Device != nullptr; }
	void Flush();
};

