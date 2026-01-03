#pragma once
#include "DX12Device.h"

class DX12CommandContext
{
private:
	bool bHasCommands = false;

	DX12Device* m_Device = nullptr;
	DX12CommandQueue* m_CommandQueue = nullptr;
	EQueueType m_QueueType{};

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> m_CommandList;

public:
	~DX12CommandContext() = default;
	DX12CommandContext() = default;

	ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList.Get(); }

	bool Initialize(DX12Device* device, EQueueType queueType);

	void MarkUsed() { bHasCommands = true; }
	void Reset();
	void Excute();
	UINT WaitFence();
	void ExcuteAndWait();
	void Close();

};

