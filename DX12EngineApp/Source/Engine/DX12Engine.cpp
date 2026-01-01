#include "../stdafx.h"
#include "DX12Engine.h"

using namespace DirectX;


DX12Engine::DX12Engine():
	m_Device(std::make_unique<Device>())
{
}

void DX12Engine::InitWindowSize(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
}

bool DX12Engine::Initialize(HWND hwnd)
{
	//m_Device = std::make_unique<Device>();
	if (!m_Device->IsInitialized())
		return false;

	return true;
}

void DX12Engine::CreateCommandComponents()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	// D3D12_COMMAND_LIST_TYPE_DIRECT 表示将命令都直接放进队列里，不做其他处理
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	m_Device->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
	
	// 创建命令分配器，它的作用是开辟内存，存储命令列表上的命令，注意命令类型要一致
	m_Device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

	m_Device->GetDevice()->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_CommandList)
	);

	m_CommandList->Close();
}

void DX12Engine::CreateRenderTarget(HWND hwnd)
{
	D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
	RTVHeapDesc.NumDescriptors = FrameCount;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_Device->GetDevice()->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&m_RTVHeap));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = FrameCount;
	swapchainDesc.Width = WindowWidth;
	swapchainDesc.Height = WindowHeight;
	swapchainDesc.Format = rtvFormat;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> temp_SwapChain;

	m_Device->GetFactory()->CreateSwapChainForHwnd(m_CommandQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&temp_SwapChain
	);

	temp_SwapChain.As(&m_DXGISwapChain);

	RTVHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

	RTVDescriptionSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0;i < FrameCount;i++)
	{
		m_DXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTarget[i]));

		m_Device->GetDevice()->CreateRenderTargetView(m_RenderTarget[i].Get(), nullptr, RTVHandle);

		RTVHandle.ptr += RTVDescriptionSize;
	}

}

void DX12Engine::CreateFenceAndBarrier()
{
	RenderEvent = CreateEvent(nullptr, false, true, nullptr);

	m_Device->GetDevice()->CreateFence(0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)
	);



	beg_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	beg_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	beg_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	end_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	end_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	end_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
}

void DX12Engine::CreatePipeline()
{
	m_Pipeline.LoadTexture();
	CreateSRVHeap();
	CreateUploadAndDefaultResource();
	CopyTextureDataToDefaultResource();
	CreateSRV();

	m_Pipeline.CreateRootSignature(m_Device->GetDevice());
	m_Pipeline.CreatePSO(m_Device->GetDevice(), rtvFormat);
	m_Pipeline.CreateVertexResource(m_Device->GetDevice(),WindowWidth,WindowHeight);
	
	
}

void DX12Engine::CreateSRVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC SRVHeapDesc = {};

	SRVHeapDesc.NumDescriptors = 1;
	SRVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_Device->GetDevice()->CreateDescriptorHeap(&SRVHeapDesc, IID_PPV_ARGS(&m_SRVHeap));

}

void DX12Engine::CreateUploadAndDefaultResource()
{
	D3D12_RESOURCE_DESC UploadResourceDesc = {};

	UploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	UploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	UploadResourceDesc.Width = m_Pipeline.GetUploadResourceSize();
	UploadResourceDesc.Height = 1;
	UploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	UploadResourceDesc.DepthOrArraySize = 1;
	UploadResourceDesc.MipLevels = 1;
	UploadResourceDesc.SampleDesc.Count = 1;

	D3D12_HEAP_PROPERTIES UploadHeapDesc = { D3D12_HEAP_TYPE_UPLOAD };

	m_Device->GetDevice()->CreateCommittedResource(&UploadHeapDesc,
		D3D12_HEAP_FLAG_NONE,
		&UploadResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_UploadTextureResource));

	D3D12_RESOURCE_DESC DefaultResourceDesc = {};

	DefaultResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DefaultResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DefaultResourceDesc.Width = m_Pipeline.GetTextureWidth();
	DefaultResourceDesc.Height = m_Pipeline.GetTextureHeight();
	DefaultResourceDesc.Format = m_Pipeline.GetTextureFormat();
	DefaultResourceDesc.DepthOrArraySize = 1;
	DefaultResourceDesc.MipLevels = 1;
	DefaultResourceDesc.SampleDesc.Count = 1;

	D3D12_HEAP_PROPERTIES DefaultHeapDesc = { D3D12_HEAP_TYPE_DEFAULT };

	m_Device->GetDevice()->CreateCommittedResource(&DefaultHeapDesc,
		D3D12_HEAP_FLAG_NONE,
		&DefaultResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_DefaultTextureResource));

}

void DX12Engine::CopyTextureDataToDefaultResource()
{
	BYTE* TextureData = (BYTE*)malloc(m_Pipeline.GetTextureSize());

	m_Pipeline.GetBitmapSource()->CopyPixels(nullptr,
		m_Pipeline.GetBytePerRowSize(),
		m_Pipeline.GetTextureSize(),
		TextureData);

	BYTE* TransferPoint = nullptr;

	m_UploadTextureResource->Map(0,
		nullptr,
		reinterpret_cast<void**>(&TransferPoint));

	for (UINT i = 0;i < m_Pipeline.GetTextureHeight();i++)
	{
		memcpy(TransferPoint, TextureData, m_Pipeline.GetBytePerRowSize());

		TextureData += m_Pipeline.GetBytePerRowSize();

		TransferPoint += m_Pipeline.GetUploadResourceRowSize();
	}

	m_UploadTextureResource->Unmap(0, nullptr);

	TextureData -= m_Pipeline.GetTextureSize();

	free(TextureData);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT PlacedFootprint = {};
	D3D12_RESOURCE_DESC DefaultResourceDesc = m_DefaultTextureResource->GetDesc();

	m_Device->GetDevice()->GetCopyableFootprints(&DefaultResourceDesc,
		0,
		1,
		0,
		&PlacedFootprint,
		nullptr,
		nullptr,
		nullptr);

	D3D12_TEXTURE_COPY_LOCATION DstLocation = {};
	DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	DstLocation.SubresourceIndex = 0;
	DstLocation.pResource = m_DefaultTextureResource.Get();

	D3D12_TEXTURE_COPY_LOCATION SrcLocation = {};
	SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	SrcLocation.PlacedFootprint = PlacedFootprint;
	SrcLocation.pResource = m_UploadTextureResource.Get();

	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);

	m_CommandList->CopyTextureRegion(&DstLocation,
		0,
		0,
		0,
		&SrcLocation,
		nullptr);

	m_CommandList->Close();

	ID3D12CommandList* _temp_cmdlists[] = { m_CommandList.Get() };

	m_CommandQueue->ExecuteCommandLists(1, _temp_cmdlists);

	FenceValue++;

	m_CommandQueue->Signal(m_Fence.Get(), FenceValue);

	m_Fence->SetEventOnCompletion(FenceValue, RenderEvent);

}

void DX12Engine::CreateSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDescriptorDesc = {};

	SRVDescriptorDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDescriptorDesc.Format = m_Pipeline.GetTextureFormat();
	SRVDescriptorDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDescriptorDesc.Texture2D.MipLevels = 1;

	SRV_CPUHandle = m_SRVHeap->GetCPUDescriptorHandleForHeapStart();

	m_Device->GetDevice()->CreateShaderResourceView(m_DefaultTextureResource.Get(),
		&SRVDescriptorDesc,
		SRV_CPUHandle);
	SRV_GPUHandle = m_SRVHeap->GetGPUDescriptorHandleForHeapStart();
}

void DX12Engine::Render()
{
	RTVHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

	FrameIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	RTVHandle.ptr += FrameIndex * RTVDescriptionSize;

	m_CommandAllocator->Reset();

	
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);

	beg_barrier.Transition.pResource = m_RenderTarget[FrameIndex].Get();

	m_CommandList->ResourceBarrier(1, &beg_barrier);

	m_CommandList->SetGraphicsRootSignature(m_Pipeline.GetRootSignature());
	m_CommandList->SetPipelineState(m_Pipeline.GetPSO());

	m_CommandList->RSSetViewports(1, &m_Pipeline.GetViewport());
	m_CommandList->RSSetScissorRects(1, &m_Pipeline.GetScissorRect());

	// 用 RTV 句柄设置渲染目标
	m_CommandList->OMSetRenderTargets(1, &RTVHandle, false, nullptr);

	// 清空当前渲染目标的背景为天蓝色
	m_CommandList->ClearRenderTargetView(RTVHandle, DirectX::Colors::SkyBlue, 0, nullptr);

	ID3D12DescriptorHeap* _temp_DescriptorHeap[] = {m_SRVHeap.Get()};
	m_CommandList->SetDescriptorHeaps(1, _temp_DescriptorHeap);
	m_CommandList->SetGraphicsRootDescriptorTable(0, SRV_GPUHandle);

	m_CommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_Pipeline.GetVBV());
	m_CommandList->DrawInstanced(6, 1, 0, 0);

	end_barrier.Transition.pResource = m_RenderTarget[FrameIndex].Get();

	m_CommandList->ResourceBarrier(1, &end_barrier);

	m_CommandList->Close();

	// 用于传递命令用的临时 ID3D12CommandList 数组
	ID3D12CommandList* _temp_cmdlists[] = { m_CommandList.Get() };

	m_CommandQueue->ExecuteCommandLists(1, _temp_cmdlists);

	m_DXGISwapChain->Present(1, NULL);

	FenceValue++;

	m_CommandQueue->Signal(m_Fence.Get(), FenceValue);

	m_Fence->SetEventOnCompletion(FenceValue, RenderEvent);

	
}


