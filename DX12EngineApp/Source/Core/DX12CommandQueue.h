#pragma once
#include "DX12Queue.h"
#include <wrl.h>

/// <summary>
/// CommandQueue 不是“提交器”，而是“GPU 的时间轴”
/// FenceValue 不是“同步工具”，而是“时间点”
/// </summary>
class DX12CommandQueue
{

private:
	EQueueType const m_type;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence;

	UINT NextFenceValue = 1;
	HANDLE m_FenceEvent = NULL;

public:
	~DX12CommandQueue();
	DX12CommandQueue(ID3D12Device* device, EQueueType type);

	ID3D12CommandQueue* GetNative() const { return m_Queue.Get(); }
	EQueueType GetType() const { return m_type; }
	HANDLE GetEventHandle() { return m_FenceEvent; }
	UINT GetNextFenceValue() { return NextFenceValue; }

	bool IsInitialized() { return m_Queue != nullptr; }

	UINT Signal();

	void Excute(UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);

	void Wait(DX12CommandQueue* otherQueue, UINT fenceValue);

	void WaitForFence(UINT fenceValue);

	UINT GetCompletedFenceValue() const;

	void Flush();
};

