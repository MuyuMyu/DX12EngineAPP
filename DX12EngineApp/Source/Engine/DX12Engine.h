#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include "../Core/Device.h"
#include "../Render/RenderPipeline.h"

class Device;

using namespace Microsoft;
using namespace Microsoft::WRL;
class DX12Engine
{
private:

	int WindowWidth = 640;		// 窗口宽度
	int WindowHeight = 480;		// 窗口高度

	static const uint32_t FrameCount = 3;
	static const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	std::unique_ptr<Device> m_Device;

	//命令组件
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> m_CommandList;

	//资源与交换链
	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_SRVHeap;

	ComPtr<IDXGISwapChain4> m_DXGISwapChain;

	ComPtr<ID3D12Resource> m_RenderTarget[3];
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
	UINT RTVDescriptionSize = 0;
	UINT FrameIndex = 0;

	ComPtr<ID3D12Resource> m_UploadTextureResource;
	ComPtr<ID3D12Resource> m_DefaultTextureResource;

	D3D12_CPU_DESCRIPTOR_HANDLE SRV_CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE SRV_GPUHandle;

	//围栏与资源屏障
	ComPtr<ID3D12Fence1> m_Fence;
	UINT FenceValue = 0;
	HANDLE RenderEvent = NULL;
	D3D12_RESOURCE_BARRIER beg_barrier = {};
	D3D12_RESOURCE_BARRIER end_barrier = {};

	RenderPipeline m_Pipeline;

public:
	DX12Engine();
	~DX12Engine() = default;

	HANDLE GetRenderEvent() const { return RenderEvent; }

public:

	void InitWindowSize(int w, int h);

	bool Initialize(HWND hwnd);

	void CreateCommandComponents();

	void CreateRenderTarget(HWND hwnd);

	void CreateFenceAndBarrier();

	void CreatePipeline();

	void CreateSRVHeap();

	void CreateUploadAndDefaultResource();

	void CopyTextureDataToDefaultResource();

	void CreateSRV();

	void Render();

};

