#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft;
using namespace Microsoft::WRL;

class RenderPipeline
{
private:
	ComPtr<ID3D12RootSignature> m_RootSignature;

public:
	 
	ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

public:

	void CreateRootSignature(ID3D12Device* device);

};

