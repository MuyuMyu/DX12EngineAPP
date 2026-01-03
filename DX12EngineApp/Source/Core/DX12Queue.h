#pragma once
#include <d3d12.h>

enum class EQueueType : UINT8
{
	Direct = 0,
	Copy,
	Compute,

	Count,

};

inline D3D12_COMMAND_LIST_TYPE GetD3DCommandListType(EQueueType type)
{
	switch (type)
	{
	case EQueueType::Direct:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case EQueueType::Copy:
		return D3D12_COMMAND_LIST_TYPE_COPY;
	case EQueueType::Compute:
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	default:
		return D3D12_COMMAND_LIST_TYPE_NONE;
	}
}