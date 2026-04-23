#ifndef WINDOW_H
#define WINDOW_H
#endif


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include<wrl.h>
#include <d3d12.h>
#include<dxgi1_5.h>

#include <Events.h>
#include <HighResolutionClock.h>
#include <string>

class Window
{
public:
	Window(const Window& other) = delete;
	Window(Window&& other) = delete;
	Window& operator=(const Window& other) = delete;
	Window& operator=(Window&& other) = delete;

	static const UINT BufferCount = 3;

	HWND GetWindowHandle() const;

	void Destroy();

	const std::wstring& GetWindowName() const;

	int GetClientWidth() const;
	int GetClientHeight() const;

	bool IsVSync() const;
	void SetVSync(bool vSync);
	void ToggleVSync();

	bool IsFullScreen() const;

	void SetFullScreen(bool fullScreen);
	void ToggleFullScreen();

	void Show();
	void Hide();

	UINT GetCurrentBackBufferIndex() const;

	UINT Present();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;

	Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

protected:
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	friend class App;
	friend class Game;

	Window() = delete;
	Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync);
	virtual ~Window();

	void RegisterCallbacks(Game* gamePtr);

	virtual void OnUpdate(UpdateEventArgs& e);
	virtual void OnRender(RenderEventArgs& e);

	//input 
	virtual void OnKeyPressed(KeyEventArgs& e);
	virtual void OnKeyReleased(KeyEventArgs& e);
	virtual void OnMouseMoved(MouseMotionEventArgs& e);
	virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
	virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
	virtual void OnMouseWheel(MouseWheelEventArgs& e);
	virtual void OnResize(ResizeEventArgs& e);

	Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain();

	void UpdateRenderTargetViews();

private:
	HWND m_hWnd{};

	std::wstring m_WindowName{};
	RECT m_WindowRect{};
	int m_ClientWidth{ 1280 };
	int m_ClientHeight{ 720 };

	bool m_VSync{ true };
	bool m_TearingSupported{ false };
	bool m_Fullscreen{ false };

	HighResolutionClock m_UpdateClock{};
	HighResolutionClock m_RenderClock{};
	uint64_t m_FrameCounter{};

	// store reference to the game ptr, no need to have any kind of ownership
	Game* m_GamePtr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap{};
	Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[BufferCount];

	UINT m_RTVDescriptorSize{};
	UINT m_CurrentBackBufferIndex{};
};