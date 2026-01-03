#pragma once
#include "DX12CommandContext.h"
class DX12FrameContext
{
private:
	DX12Device* Device = nullptr;
	UINT fenceValue = 0;

	std::unique_ptr<DX12CommandContext> m_CopyContext;
	std::unique_ptr<DX12CommandContext> m_GrapicsContext;

public:
	~DX12FrameContext() = default;
	DX12FrameContext(DX12Device* device);

	DX12CommandContext* GetGraphicsContext() { return m_GrapicsContext.get(); }
	DX12CommandContext* GetCopyContext() { return m_CopyContext.get(); }

	void Begin();
	void End();
};

