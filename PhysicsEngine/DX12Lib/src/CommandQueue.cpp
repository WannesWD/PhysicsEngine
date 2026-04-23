#include "PCH.h"
#include "CommandQueue.h"


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
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator{};
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList{};

	if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
	{
		commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;

		m_CommandAllocatorQueue.pop();
		ThrowIfFailed(commandAllocator->Reset());
	}
	else
	{
		commandAllocator = CreateCommandAllocator();
	}

	if (!m_CommandListQueue.empty())
	{
		commandList = m_CommandListQueue.front();
		
		m_CommandListQueue.pop();
		ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
	}

	ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

	return commandList;
}

uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	commandList->Close();

	ID3D12CommandAllocator* commandAllocator{};
	UINT dataSize = sizeof(commandAllocator);
	ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

	ID3D12CommandList* const ppCommandLists[]{ commandList.Get() };

	m_CommandQueue->ExecuteCommandLists(1, ppCommandLists);
	uint64_t fenceValue = Signal();

	m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
	m_CommandListQueue.push(commandList);

	commandAllocator->Release();

	return fenceValue;
}

uint64_t CommandQueue::Signal()
{
	uint64_t fenceValueForSignal = ++m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValueForSignal));

	return fenceValueForSignal;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return m_Fence->GetCompletedValue() >= m_FenceValue;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	if (!IsFenceComplete(fenceValue))
	{
		m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
		::WaitForSingleObject(m_FenceEvent, DWORD_MAX);
	}
}

void CommandQueue::Flush()
{
	WaitForFenceValue(Signal());
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const
{

}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	ThrowIfFailed(m_Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator)));

	return commandAllocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;
	ThrowIfFailed(m_Device->CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

	ThrowIfFailed(commandList->Close());

	return commandList;
}
