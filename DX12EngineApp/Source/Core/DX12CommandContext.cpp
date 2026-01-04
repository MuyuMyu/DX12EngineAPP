#include "../stdafx.h"
#include "DX12CommandContext.h"

bool DX12CommandContext::Initialize(DX12Device* device, EQueueType queueType)
{
	m_Device = device;
	m_CommandQueue = m_Device->GetCommandQueue(queueType);
	m_QueueType = queueType;

	auto d3dDevice = m_Device->GetDevice();
	auto cmdListType = GetD3DCommandListType(m_QueueType);

	HRESULT allocatorHr = d3dDevice->CreateCommandAllocator(cmdListType, IID_PPV_ARGS(&m_CommandAllocator));

	if(FAILED(allocatorHr))	return false;

	HRESULT listHr = d3dDevice->CreateCommandList(0,
		cmdListType,
		m_CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_CommandList));

	if (FAILED(listHr)) return false;


	m_CommandList->Close();

	return true;
}

void DX12CommandContext::Reset()
{
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
}

UINT DX12CommandContext::Excute()
{
	ID3D12CommandList* Lists[] = { m_CommandList.Get() };
	m_CommandQueue->Excute(1, Lists);
	return m_CommandQueue->Signal();
}

UINT DX12CommandContext::WaitFence()
{
	const UINT fenceValue = m_CommandQueue->Signal();
	m_CommandQueue->WaitForFence(fenceValue);

	return fenceValue;
}

UINT DX12CommandContext::ExcuteAndWait()
{
	ID3D12CommandList* Lists[] = { m_CommandList.Get() };
	m_CommandQueue->Excute(1, Lists);
	const UINT fenceValue = m_CommandQueue->Signal();
	m_CommandQueue->WaitForFence(fenceValue);
	 
	return fenceValue;
}

void DX12CommandContext::Close()
{
	bHasCommands = false;
	m_CommandList->Close();
}
