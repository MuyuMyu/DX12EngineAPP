#pragma once

#include "../Render/RenderPipeline.h"
#include "../Config/EngineConfig.h"
#include "../Core/DX12CommandContext.h"


using namespace Microsoft;
using namespace Microsoft::WRL;
class DX12Engine
{
private:

	int WindowWidth = 640;		// 窗口宽度
	int WindowHeight = 480;		// 窗口高度

	std::unique_ptr<DX12Device> m_Device;
	std::unique_ptr<DX12CommandContext> m_CommandContext;

	//资源与交换链
	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_SRVHeap;

	ComPtr<IDXGISwapChain4> m_DXGISwapChain;

	ComPtr<ID3D12Resource> m_RenderTarget[FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
	UINT RTVDescriptionSize = 0;
	UINT FrameIndex = 0;

	ComPtr<ID3D12Resource> m_UploadTextureResource;
	ComPtr<ID3D12Resource> m_DefaultTextureResource;

	D3D12_CPU_DESCRIPTOR_HANDLE SRV_CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE SRV_GPUHandle;

	//围栏与资源屏障
	D3D12_RESOURCE_BARRIER beg_barrier = {};
	D3D12_RESOURCE_BARRIER end_barrier = {};

	RenderPipeline m_Pipeline;

public:
	DX12Engine();
	~DX12Engine();

	HANDLE GetRenderEvent(EQueueType type) const { return m_Device->GetCommandQueue(type)->GetEventHandle(); }

public:

	void InitWindowSize(int w, int h);

	bool Initialize(HWND hwnd);

	void CreateRenderTarget(HWND hwnd);

	void CreateFenceAndBarrier();

	void CreatePipeline();

	void CreateSRVHeap();

	void CreateUploadAndDefaultResource();

	void CopyTextureDataToDefaultResource();

	void CreateSRV();

	void Render();

};

