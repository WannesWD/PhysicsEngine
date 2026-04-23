#ifndef APP_H
#define APP_H
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <memory>
#include <string>

class Window;
class Game;
class CommandQueue;

namespace Application
{
	class App
	{
	public:
		App(const App& other) = delete;
		App(App&& other) = delete;
		App& operator=(const App& other) = delete;
		App& operator=(App&& other) = delete;

		static void Create(HINSTANCE hInst);
		static void Destroy();
		static const App& Get();

		bool IsTearingSupported() const;
		Window* const CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);

		void DestroyWindow(const std::wstring& windowName);
		void DestroyWindow(Window* const window);

		Window* const GetWindowByName(const std::wstring& windowName);

		int Run(Game* const gamePtr);
		void Quit(int exitCode = 0);

		Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const;
		CommandQueue* const GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

		void Flush();

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

	protected:

		Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool bUseWarp);
		Microsoft::WRL::ComPtr<ID3D12Device2> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
		bool CheckTearingSupport();

	private:
		App(HINSTANCE hInst);
		virtual ~App();

		HINSTANCE m_hInstance{};

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter{};
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device{};

		std::unique_ptr<CommandQueue> m_DirectCommandQueuePtr{};
		std::unique_ptr<CommandQueue> m_ComputeCommandQueuePtr{};
		std::unique_ptr<CommandQueue> m_CopyCommandQueuePtr{};

		bool m_TearingSupported{};
	};
}
