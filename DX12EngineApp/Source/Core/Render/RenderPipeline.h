#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wincodec.h>
#include <string>

using namespace DirectX;
using namespace Microsoft;
using namespace Microsoft::WRL;

class RenderPipeline
{
private:
	struct VERTEX
	{
		XMFLOAT4 position;
		XMFLOAT4 color;
	};

private:

	ComPtr<ID3D12RootSignature> m_RootSignature;
	ComPtr<ID3D12PipelineState> m_PipelineStateObject;
	ComPtr<ID3D12Resource> m_VertexResource;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT ScissorRect;

	ComPtr<IWICImagingFactory> m_WICFactory;
	std::wstring TextureFilename = L"Assets/Textures/diamond_ore.png";
	ComPtr<IWICBitmapDecoder> m_WICBitmapDecoder;

public:
	 
	ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }
	ID3D12PipelineState* GetPSO() const { return m_PipelineStateObject.Get(); }
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return VertexBufferView; }
	const D3D12_VIEWPORT& GetViewport() const { return viewPort; }
	const D3D12_RECT& GetScissorRect() const { return ScissorRect; }

public:

	void CreateRootSignature(ID3D12Device* device);

	void CreatePSO(ID3D12Device* device,DXGI_FORMAT rtvFormat);

	void CreateVertexResource(ID3D12Device* device,int w,int h);

	void LoadTexture();
};

