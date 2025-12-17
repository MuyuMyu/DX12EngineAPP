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

void RenderPipeline::LoadTexture()
{
	if (m_WICFactory == nullptr) CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_WICFactory));

	HRESULT hr = m_WICFactory->CreateDecoderFromFilename(TextureFilename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&m_WICBitmapDecoder
	);

	//std::wostringstream output_str;		// 用于格式化字符串
	//switch (hr)
	//{
	//case S_OK: break;	// 解码成功，直接 break 进入下一步即可

	//case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):	// 文件找不到
	//	output_str << L"找不到文件 " << TextureFilename << L" ！请检查文件路径是否有误！";
	//	MessageBox(NULL, output_str.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
	//	return false;

	//case HRESULT_FROM_WIN32(ERROR_FILE_CORRUPT):	// 文件句柄正在被另一个应用进程占用
	//	output_str << L"文件 " << TextureFilename << L" 已经被另一个应用进程打开并占用了！请先关闭那个应用进程！";
	//	MessageBox(NULL, output_str.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
	//	return false;

	//case WINCODEC_ERR_COMPONENTNOTFOUND:			// 找不到可解码的组件，说明这不是有效的图像文件
	//	output_str << L"文件 " << TextureFilename << L" 不是有效的图像文件，无法解码！请检查文件是否为图像文件！";
	//	MessageBox(NULL, output_str.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
	//	return false;

	//default:			// 发生其他未知错误
	//	output_str << L"文件 " << TextureFilename << L" 解码失败！发生了其他错误，错误码：" << hr << L" ，请查阅微软官方文档。";
	//	MessageBox(NULL, output_str.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
	//	return false;
	//}

}
