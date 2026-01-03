#pragma once

#include <cstdint>

// ==================== 全局渲染配置 ====================

// 帧缓冲数量（双缓冲或三缓冲）
constexpr uint32_t FrameCount = 3;                  

// 默认窗口尺寸（可在运行时修改，但这里提供默认值）
constexpr int32_t DefaultWindowWidth = 720;
constexpr int32_t DefaultWindowHeight = 720;

// 交换链和渲染目标格式
constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

// 调试相关
constexpr bool EnableDebugLayer = true;             // 可在 CMake 或编译宏中控制
constexpr bool EnableGPUBasedValidation = true;     // 调试时强烈推荐打开

// 其他通用配置（后续可扩展）
constexpr bool EnableVSync = true;
constexpr uint32_t MaxDescriptorCount_SRV_CBV_UAV = 10000;  // 大描述符堆大小