#include "../../stdafx.h"
#include "RenderPipeline.h"


void RenderPipeline::CreateRootSignature(ID3D12Device* device)
{
	ComPtr<ID3DBlob> SignatureBlob;
	ComPtr<ID3DBlob> ErrorBlob;

	D3D12_ROOT_SIGNATURE_DESC rootsignatureDesc = {};
	rootsignatureDesc.NumParameters = 0;
	rootsignatureDesc.pParameters = nullptr;
	rootsignatureDesc.NumStaticSamplers = 0;
	rootsignatureDesc.pStaticSamplers = nullptr;
	rootsignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12SerializeRootSignature(&rootsignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_1,
		SignatureBlob.GetAddressOf(),
		ErrorBlob.GetAddressOf());


	if (ErrorBlob)
	{
		OutputDebugStringA((const char*)ErrorBlob->GetBufferPointer());
		OutputDebugStringA("\n");

	}

	device->CreateRootSignature(0,
		SignatureBlob->GetBufferPointer(),
		SignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));
}
