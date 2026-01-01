#pragma once
#include "../stdafx.h"

class Device
{

private:
	Microsoft::WRL::ComPtr<ID3D12Debug6> m_DebugDevice;
	UINT m_DXGICreateFactoryFlag = NULL;
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
	Microsoft::WRL::ComPtr<ID3D12Device14> m_Device;

public:

	Device();
	~Device() = default;

	IDXGIFactory7* GetFactory()  { return m_Factory.Get(); }
	IDXGIAdapter4* GetAdapter()  { return m_Adapter.Get(); }
	ID3D12Device14* GetDevice()  { return m_Device.Get(); }
	bool IsInitialized() { return m_Device != nullptr; }

};

