#include "../stdafx.h"
#include "DX12FrameContext.h"

DX12FrameContext::DX12FrameContext(DX12Device* device):
	Device(device),
	m_CopyContext(std::make_unique<DX12CommandContext>()),
	m_GrapicsContext(std::make_unique<DX12CommandContext>())
{
	m_CopyContext->Initialize(Device, EQueueType::Direct);
	m_GrapicsContext->Initialize(Device, EQueueType::Direct);
}

void DX12FrameContext::Begin()
{
	if (fenceValue > 0)
	{
		Device->GetCommandQueue(EQueueType::Direct)->WaitForFence(fenceValue);
	}

	m_GrapicsContext->Reset();
	m_CopyContext->Reset();
}

void DX12FrameContext::End()
{
	UINT copyFenceValue = 0;
	if (m_CopyContext->HasCommands())
	{
		m_CopyContext->Close();
		copyFenceValue =  m_CopyContext->Excute();

	}

	if (m_GrapicsContext->HasCommands())
	{
		if (copyFenceValue > 0)
		{
			Device->GetCommandQueue(EQueueType::Direct)
				->Wait(Device->GetCommandQueue(EQueueType::Copy), copyFenceValue);
		}
		m_GrapicsContext->Close();
		fenceValue = m_GrapicsContext->Excute();
	}

}
