#include "CommandQueue.h"
#include "Helpers.h"
#include <assert.h>

CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	: m_FenceValue{ 0 },
	m_CommandListType{ type },
	m_Device{ device }
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)));
	ThrowIfFailed(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_FenceEvent && "Failed to create fence event handle.");
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
{
	return Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>();
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList2 commandList)
{
	return 0;
}

uint64_t CommandQueue::Signal()
{
	return 0;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return false;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
}

void CommandQueue::Flush()
{
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const
{
	return Microsoft::WRL::ComPtr<ID3D12CommandQueue>();
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
{
	return Microsoft::WRL::ComPtr<ID3D12CommandAllocator>();
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
{
	return Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>();
}
