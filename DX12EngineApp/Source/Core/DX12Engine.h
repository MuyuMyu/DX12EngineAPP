#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "./Render/RenderPipeline.h"

using namespace Microsoft;
using namespace Microsoft::WRL;
class DX12Engine
{
private:

	int WindowWidth = 640;		// 窗口宽度
	int WindowHeight = 480;		// 窗口高度

	static const uint32_t FrameCount = 3;
	static const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	//调试层
	ComPtr<ID3D12Debug6> m_D3D12DebugDevice;
	UINT m_DXGICreateFactoryFlag = NULL;

	//设备层
	ComPtr<IDXGIFactory7> m_DXGIFactory;
	ComPtr<IDXGIAdapter4> m_DXGIAdapter;
	ComPtr<ID3D12Device14> m_D3D12Device;

	//命令组件
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList10> m_CommandList;

	//资源与交换链
	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	ComPtr<IDXGISwapChain4> m_DXGISwapChain;
	ComPtr<ID3D12Resource> m_RenderTarget[3];
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
	UINT RTVDescriptionSize = 0;
	UINT FrameIndex = 0;

	//围栏与资源屏障
	ComPtr<ID3D12Fence1> m_Fence;
	UINT FenceValue = 0;
	HANDLE RenderEvent = NULL;
	D3D12_RESOURCE_BARRIER beg_barrier = {};
	D3D12_RESOURCE_BARRIER end_barrier = {};

	RenderPipeline m_Pipeline;

public:

	HANDLE GetRenderEvent() const { return RenderEvent; }

public:

	void InitWindowSize(int w, int h);

	void CreateDebugDevice();

	bool CreateDevice();

	void CreateCommandComponents();

	void CreateRenderTarget(HWND hwnd);

	void CreateFenceAndBarrier();

	void CreatePipeline();

	void Render();

};

