#include "../../stdafx.h"
#include "RenderPipeline.h"
#include "../../Util/Resource/TextureResourceUtil.h"

using namespace DX12EngineUtils;
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

	HRESULT hr = D3D12SerializeRootSignature(&rootsignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		SignatureBlob.GetAddressOf(),
		ErrorBlob.GetAddressOf());


	if (FAILED(hr))
	{
		if (ErrorBlob)
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
		return;
	}

	HRESULT rsHr = device->CreateRootSignature(0,
		SignatureBlob->GetBufferPointer(),
		SignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));

	if (FAILED(rsHr))
	{
		assert(false);
		return;
	}
}

void RenderPipeline::CreatePSO(ID3D12Device* device, DXGI_FORMAT rtvFormat)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
	D3D12_INPUT_ELEMENT_DESC InputElementDesc[2] = {};

	InputElementDesc[0].SemanticName = "POSITION";
	InputElementDesc[0].SemanticIndex = 0;
	InputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	InputElementDesc[0].InputSlot = 0;
	InputElementDesc[0].AlignedByteOffset = 0;
	InputElementDesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	InputElementDesc[0].InstanceDataStepRate = 0;

	InputElementDesc[1].SemanticName = "COLOR";
	InputElementDesc[1].SemanticIndex = 0;
	InputElementDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	InputElementDesc[1].InputSlot = 0;
	InputElementDesc[1].AlignedByteOffset = 16;
	InputElementDesc[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	InputElementDesc[1].InstanceDataStepRate = 0;

	InputLayoutDesc.NumElements = 2;
	InputLayoutDesc.pInputElementDescs = InputElementDesc;

	PSODesc.InputLayout = InputLayoutDesc;

	ComPtr<ID3DBlob> VertexShaderBlob;
	ComPtr<ID3DBlob> PixelShadrBlob;
	ComPtr<ID3DBlob> ErrorBlob;

	HRESULT vsHr = D3DCompileFromFile(L"Assets/Shaders/shader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_1",
		NULL,
		NULL,
		&VertexShaderBlob,
		&ErrorBlob);

	if (FAILED(vsHr))
	{
		if (ErrorBlob)
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
		assert(false);
		return;
	}

	HRESULT psHr = D3DCompileFromFile(L"Assets/Shaders/shader.hlsl",
		nullptr,
		nullptr,
		"PSMain",
		"ps_5_1",
		NULL,
		NULL,
		&PixelShadrBlob,
		&ErrorBlob);
	if (FAILED(psHr))
	{
		if (ErrorBlob)
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
		assert(false);
		return;
	}

	PSODesc.VS.BytecodeLength = VertexShaderBlob->GetBufferSize();
	PSODesc.VS.pShaderBytecode = VertexShaderBlob->GetBufferPointer();
	PSODesc.PS.BytecodeLength = PixelShadrBlob->GetBufferSize();
	PSODesc.PS.pShaderBytecode = PixelShadrBlob->GetBufferPointer();

	PSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	PSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	PSODesc.pRootSignature = m_RootSignature.Get();

	PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	PSODesc.NumRenderTargets = 1;
	PSODesc.RTVFormats[0] = rtvFormat;

	PSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	PSODesc.SampleDesc.Count = 1;

	PSODesc.SampleMask = UINT_MAX;

	PSODesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	HRESULT psoHr = device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&m_PipelineStateObject));

	if (FAILED(psoHr))
	{
		assert(false);
	}
}

void RenderPipeline::CreateVertexResource(ID3D12Device* device, int w, int h)
{
	//VERTEX vertexs[3] =
	//{
	//	{{0.0f,0.5f,0.0f,1.0f},XMFLOAT4(Colors::Blue)},
	//	{{0.75f,-0.5f,0.0f,1.0f},XMFLOAT4(Colors::Red)},
	//	{{-0.75f,-0.5f,0.0f,1.0f},XMFLOAT4(Colors::Green)},
	//};

	VERTEX vertexs[] =
	{
		{{-0.5f,0.5f,0.0f,1.0f},XMFLOAT4(Colors::Blue)},
		{{0.5f,0.5f,0.0f,1.0f},XMFLOAT4(Colors::Yellow)},
		{{0.5f,-0.5f,0.0f,1.0f},XMFLOAT4(Colors::Red)},
		{{-0.5f,0.5f,0.0f,1.0f},XMFLOAT4(Colors::Blue)},
		{{0.5f,-0.5f,0.0f,1.0f},XMFLOAT4(Colors::Red)},
		{{-0.5f,-0.5f,0.0f,1.0f},XMFLOAT4(Colors::Green)},
	};

	D3D12_RESOURCE_DESC VertexDesc = {};
	VertexDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	VertexDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	VertexDesc.Width = sizeof(vertexs);
	VertexDesc.Height = 1;
	VertexDesc.Format = DXGI_FORMAT_UNKNOWN;
	VertexDesc.DepthOrArraySize = 1;
	VertexDesc.MipLevels = 1;
	VertexDesc.SampleDesc.Count = 1;

	D3D12_HEAP_PROPERTIES UploadHeapDesc = { D3D12_HEAP_TYPE_UPLOAD };

	device->CreateCommittedResource(&UploadHeapDesc,
		D3D12_HEAP_FLAG_NONE,
		&VertexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_VertexResource));

	BYTE* TransferPointer = nullptr;

	m_VertexResource->Map(0,
		nullptr,
		reinterpret_cast<void**>(&TransferPointer));
	memcpy(TransferPointer, vertexs, sizeof(vertexs));

	m_VertexResource->Unmap(0, nullptr);

	VertexBufferView.BufferLocation = m_VertexResource->GetGPUVirtualAddress();
	VertexBufferView.SizeInBytes = sizeof(vertexs);
	VertexBufferView.StrideInBytes = sizeof(VERTEX);

	viewPort.Height = float(h);
	viewPort.Width = float(w);
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.MinDepth = D3D12_MIN_DEPTH;
	viewPort.MaxDepth = D3D12_MAX_DEPTH;

	ScissorRect.left = 0;
	ScissorRect.top = 0;
	ScissorRect.right = w;
	ScissorRect.bottom = h;


}

bool RenderPipeline::LoadTexture()
{
	if (m_WICFactory == nullptr)
	{
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,  // 必须是这个，固定不变
			nullptr,                  // 不聚合，通常为 nullptr
			CLSCTX_INPROC_SERVER,     // 在进程内服务器加载（最常用）
			IID_PPV_ARGS(&m_WICFactory)
		);

		if (FAILED(hr))
		{
			// 处理创建失败的情况（比如系统缺少 WIC 组件，老版本 Windows）
			MessageBox(nullptr, L"Failed to create WIC Imaging Factory!", L"Error", MB_OK | MB_ICONERROR);
			return false;
		}
	}

	HRESULT hr = m_WICFactory->CreateDecoderFromFilename(TextureFilename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&m_WICBitmapDecoder
	);

	std::wostringstream output_str;		// Used for formatting strings
	switch (hr)
	{
	case S_OK: break;	// Decoding successful, just break to proceed

	case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):	// File not found
		output_str << L"Cannot find file " << TextureFilename << L"! Please check if the file path is correct!";
		MessageBox(NULL, output_str.str().c_str(), L"Error", MB_OK | MB_ICONERROR);
		return false;

	case HRESULT_FROM_WIN32(ERROR_FILE_CORRUPT):	// Note: ERROR_FILE_CORRUPT is not the correct code for "file in use"
		// Actually, the intended error for "file locked by another process" is usually ERROR_SHARING_VIOLATION or ERROR_ACCESS_DENIED
		output_str << L"File " << TextureFilename << L" is already opened and locked by another application! Please close that application first!";
		MessageBox(NULL, output_str.str().c_str(), L"Error", MB_OK | MB_ICONERROR);
		return false;

	case WINCODEC_ERR_COMPONENTNOTFOUND:			// No decoder component found → not a valid image file
		output_str << L"File " << TextureFilename << L" is not a valid image file and cannot be decoded! Please verify it is an image file!";
		MessageBox(NULL, output_str.str().c_str(), L"Error", MB_OK | MB_ICONERROR);
		return false;

	default:			// Other unknown error occurred
		output_str << L"Failed to decode file " << TextureFilename << L"! An unknown error occurred. Error code: " << hr << L". Please refer to Microsoft official documentation.";
		MessageBox(NULL, output_str.str().c_str(), L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_WICBitmapDecoder->GetFrame(0, &m_WICBitmapDecodeFrame);

	WICPixelFormatGUID SourceFormat = {};
	GUID TargeFormat = {};

	m_WICBitmapDecodeFrame->GetPixelFormat(&SourceFormat);

	if (GetTargetPixelFormat(&SourceFormat, &TargeFormat))
	{
		TextureFormat = GetDXGIFormatFromPixelFormat(&TargeFormat);
	}
	else
	{
		::MessageBox(NULL, L"Texture Not Support!", L"Warring", MB_OK);
		return false;
	}

	m_WICFactory->CreateFormatConverter(&m_WICFormatConverter);

	m_WICFormatConverter->Initialize(m_BitmapSource.Get(),
		TargeFormat,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom);

	m_WICFormatConverter.As(&m_BitmapSource);

	m_BitmapSource->GetSize(&TextureWidth, &TextureHight);

	ComPtr<IWICComponentInfo> _temp_WICComponentInfo = {};
	ComPtr<IWICPixelFormatInfo> _temp_WICPixelFormatInfo = {};

	m_WICFactory->CreateComponentInfo(TargeFormat, &_temp_WICComponentInfo);
	_temp_WICComponentInfo.As(&_temp_WICPixelFormatInfo);
	_temp_WICPixelFormatInfo->GetBitsPerPixel(&BitsPerPixel);

}
