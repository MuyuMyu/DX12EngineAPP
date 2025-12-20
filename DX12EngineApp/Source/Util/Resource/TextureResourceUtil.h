#pragma once
#include <wincodec.h>
#include <dxgiformat.h>
#include <sstream>

namespace DX12EngineUtils
{

	// 纹理转换用，不是 DX12 所支持的格式，DX12 没法用

	// Standard GUID -> DXGI 格式转换结构体
	struct WICTranslate
	{
		GUID wic;
		DXGI_FORMAT format;
	};

	// WIC 格式与 DXGI 像素格式的对应表，该表中的格式为被支持的格式
	static WICTranslate g_WICFormats[] =
	{
		{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },
		{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM },
		{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
		{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },
		{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
		{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },
		{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
		{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
		{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
		{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },
		{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM }
	};

	// GUID -> Standard GUID 格式转换结构体
	struct WICConvert
	{
		GUID source;
		GUID target;
	};

	// WIC 像素格式转换表
	static WICConvert g_WICConvert[] =
	{
		// 目标格式一定是最接近的被支持的格式
		{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM
		{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM
		{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM
		{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf },		// DXGI_FORMAT_R16_FLOAT
		{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat },	// DXGI_FORMAT_R32_FLOAT
		{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 },		// DXGI_FORMAT_B5G5R5A1_UNORM
		{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 },	// DXGI_FORMAT_R10G10B10A2_UNORM
		{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT

		{ GUID_WICPixelFormat128bppRGBAFloat,       GUID_WICPixelFormat128bppRGBAFloat },	// DXGI_FORMAT_R32G32B32A32_FLOAT
		{ GUID_WICPixelFormat64bppRGBAHalf,         GUID_WICPixelFormat64bppRGBAHalf },		// DXGI_FORMAT_R16G16B16A16_FLOAT
		{ GUID_WICPixelFormat64bppRGBA,             GUID_WICPixelFormat64bppRGBA },			// DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat32bppRGBA,             GUID_WICPixelFormat32bppRGBA },			// DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat32bppBGRA,             GUID_WICPixelFormat32bppBGRA },			// DXGI_FORMAT_B8G8R8A8_UNORM
		{ GUID_WICPixelFormat32bppBGR,              GUID_WICPixelFormat32bppBGR },			// DXGI_FORMAT_B8G8R8X8_UNORM
		{ GUID_WICPixelFormat32bppRGBA1010102XR,    GUID_WICPixelFormat32bppRGBA1010102XR },// DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM
		{ GUID_WICPixelFormat32bppRGBA1010102,      GUID_WICPixelFormat32bppRGBA1010102 },	// DXGI_FORMAT_R10G10B10A2_UNORM
		{ GUID_WICPixelFormat16bppBGRA5551,         GUID_WICPixelFormat16bppBGRA5551 },		// DXGI_FORMAT_B5G5R5A1_UNORM
		{ GUID_WICPixelFormat16bppBGR565,           GUID_WICPixelFormat16bppBGR565 },		// DXGI_FORMAT_B5G6R5_UNORM
		{ GUID_WICPixelFormat32bppGrayFloat,        GUID_WICPixelFormat32bppGrayFloat },	// DXGI_FORMAT_R32_FLOAT
		{ GUID_WICPixelFormat16bppGrayHalf,         GUID_WICPixelFormat16bppGrayHalf },		// DXGI_FORMAT_R16_FLOAT
		{ GUID_WICPixelFormat16bppGray,             GUID_WICPixelFormat16bppGray },			// DXGI_FORMAT_R16_UNORM
		{ GUID_WICPixelFormat8bppGray,              GUID_WICPixelFormat8bppGray },			// DXGI_FORMAT_R8_UNORM
		{ GUID_WICPixelFormat8bppAlpha,             GUID_WICPixelFormat8bppAlpha }			// DXGI_FORMAT_A8_UNORM
	};


	// 查表确定兼容的最接近格式是哪个
	bool GetTargetPixelFormat(const GUID* pSourceFormat, GUID* pTargetFormat)
	{
		*pTargetFormat = *pSourceFormat;
		for (size_t i = 0; i < _countof(g_WICConvert); ++i)
		{
			if (InlineIsEqualGUID(g_WICConvert[i].source, *pSourceFormat))
			{
				*pTargetFormat = g_WICConvert[i].target;
				return true;
			}
		}
		return false;		// 找不到，就返回 false
	}

	// 查表确定最终对应的 DXGI 格式是哪一个
	DXGI_FORMAT GetDXGIFormatFromPixelFormat(const GUID* pPixelFormat)
	{
		for (size_t i = 0; i < _countof(g_WICFormats); ++i)
		{
			if (InlineIsEqualGUID(g_WICFormats[i].wic, *pPixelFormat))
			{
				return g_WICFormats[i].format;
			}
		}
		return DXGI_FORMAT_UNKNOWN;		// 找不到，就返回 UNKNOWN
	}

	// 暴露一个统一的转换接口
	inline DXGI_FORMAT GetCompatibleDXGIFormat(const GUID& wicFormat) {
		GUID targetGuid;
		if (DX12EngineUtils::GetTargetPixelFormat(&wicFormat, &targetGuid)) {
			DXGI_FORMAT format = DX12EngineUtils::GetDXGIFormatFromPixelFormat(&targetGuid);

			if (format != DXGI_FORMAT_UNKNOWN) {
				return format;
			}
		}

		// --- 错误处理逻辑 ---

		// 将 GUID 转换为字符串，方便排查具体是什么格式不支持
		OLECHAR* guidString;
		StringFromCLSID(wicFormat, &guidString);

		std::wstringstream ss;
		ss << L"不支持的纹理格式！\n"
			<< L"WIC Pixel Format GUID: " << guidString << L"\n"
			<< L"请检查图片是否为标准格式，或在 DX12TextureHelper 中添加该格式映射。";

		MessageBoxW(NULL, ss.str().c_str(), L"纹理加载错误", MB_OK | MB_ICONERROR);

		// 释放 COM 字符串内存
		CoTaskMemFree(guidString);

		return DXGI_FORMAT_UNKNOWN;
	}

}