#include "../stdafx.h"
#include "DX12CommandQueue.h"

DX12CommandQueue::~DX12CommandQueue()
{
	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = NULL;
	}
}

DX12CommandQueue::DX12CommandQueue(ID3D12Device* device, EQueueType type) :
	m_type(type)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = GetD3DCommandListType(m_type);

	if (FAILED(device->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&m_Queue))))
	{
		MessageBox(
			NULL,
			L"CommandQueue Init Failed!",
			L"Error",
			MB_OK | MB_ICONERROR
		);
	}

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
	{
		MessageBox(
			NULL,
			L"Fence Init Failed!",
			L"Error",
			MB_OK | MB_ICONERROR
		);
	}

	m_FenceEvent = CreateEvent(nullptr, FALSE, TRUE,nullptr);
	assert(m_FenceEvent && "Failed to create fence event");
}

UINT DX12CommandQueue::Signal()
{
	const UINT fenceValue = NextFenceValue++;
	m_Queue->Signal(m_Fence.Get(), fenceValue);

	return fenceValue;
}

void DX12CommandQueue::Excute(UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists)
{

	m_Queue->ExecuteCommandLists(NumCommandLists, ppCommandLists);

}

void DX12CommandQueue::Wait(DX12CommandQueue& otherQueue, UINT fenceValue)
{
	m_Queue->Wait(otherQueue.m_Fence.Get(), fenceValue);
}

void DX12CommandQueue::WaitForFence(UINT fenceValue)
{
	if (m_Fence->GetCompletedValue() < fenceValue)
	{
		m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
	}
}

UINT DX12CommandQueue::GetCompletedFenceValue() const
{
	return m_Fence->GetCompletedValue();
}

void DX12CommandQueue::Flush()
{
	const UINT fenceValue = Signal();
	WaitForFence(fenceValue);
}


