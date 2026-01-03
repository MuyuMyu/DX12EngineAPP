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
	m_GrapicsContext->Close();
	m_GrapicsContext->ExcuteAndWait();

	fenceValue = Device->GetCommandQueue(EQueueType::Direct)->Signal();
}
