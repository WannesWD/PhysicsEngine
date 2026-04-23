#include "PCH.h"
#include "Window.h"
#include "App.h"
#include "CommandQueue.h"
#include "Game.h"


Window::Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
	: m_hWnd{ hWnd },
	m_WindowName{ windowName },
	m_ClientWidth{ clientWidth },
	m_ClientHeight{ clientHeight },
	m_VSync{ vSync },
	m_Fullscreen{ false },
	m_FrameCounter{ 0 }
{
}

Window::~Window()
{
	assert(!m_hWnd && "Use Application::DestroyWindow before destruction.");
}

void Window::RegisterCallbacks(Game* gamePtr)
{
	m_GamePtr = gamePtr;
}

void Window::OnUpdate(UpdateEventArgs& e)
{
	m_UpdateClock.Tick();

	if (auto gamePtr = m_GamePtr.lock())
	{
		m_FrameCounter++;

		UpdateEventArgs updateEventArgs(m_UpdateClock.GetDeltaSeconds(), m_UpdateClock.GetTotalSeconds());
		gamePtr->OnUpdate(updateEventArgs);
	}
}

void Window::OnRender(RenderEventArgs& e)
{
	m_RenderClock.Tick();
	if (auto gamePtr = m_GamePtr.lock())
	{
		RenderEventArgs renderEventArgs(m_RenderClock.GetDeltaSeconds(), m_RenderClock.GetTotalSeconds());
		gamePtr->OnRender(renderEventArgs);
	}
}

void Window::OnKeyPressed(KeyEventArgs& e)
{
	if (auto pGame = m_GamePtr.lock())
	{
		pGame->OnKeyPressed(e);
	}
}

void Window::OnKeyReleased(KeyEventArgs& e)
{
	if (auto pGame = m_GamePtr.lock())
	{
		pGame->OnKeyReleased(e);
	}
}

void Window::OnMouseMoved(MouseMotionEventArgs& e)
{
}

void Window::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
}

void Window::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
}

void Window::OnMouseWheel(MouseWheelEventArgs& e)
{
}

void Window::OnResize(ResizeEventArgs& e)
{
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Window::CreateSwapChain()
{
	ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;

#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = m_ClientWidth;
	swapChainDesc.Height = m_ClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = m_TearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	ID3D12CommandQueue* pCommandQueue = app.GetCommandQueue()->GetD3D12CommandQueue().Get();


	ComPtr<IDXGISwapChain1> swapChain1{};
	ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
		pCommandQueue.Get(),
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1));

	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	m_CurrentBackBufferIndex = dxgiSwapChain4->GetCurrentBackBufferIndex();

	return dxgiSwapChain4;
}

void Window::UpdateRenderTargetViews()
{
	auto devixe = App::Get().GetDevice();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (size_t i = 0; i < BufferCount; i++)
	{
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
		m_BackBuffers[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}

HWND Window::GetWindowHandle() const
{
	return m_hWnd;
}

void Window::Destroy()
{
	if (auto pGame = m_pGame.lock())
	{
		// Notify the registered game that the window is being destroyed.
		pGame->OnWindowDestroy();
	}
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

const std::wstring& Window::GetWindowName() const
{
	return m_WindowName;
}

int Window::GetClientWidth() const
{
	return m_ClientWidth;
}

int Window::GetClientHeight() const
{
	return m_ClientHeight;
}

bool Window::IsVSync() const
{
	return m_VSync;
}

void Window::SetVSync(bool vSync)
{
	m_VSync = vSync;
}

void Window::ToggleVSync()
{
	SetVSync(!m_VSync);
}

bool Window::IsFullScreen() const
{
	return m_Fullscreen;
}

void Window::SetFullScreen(bool fullScreen)
{
	if (m_Fullscreen != fullScreen)
	{
		m_Fullscreen = fullScreen;

		if (m_Fullscreen)
		{
			::GetWindowRect(m_hWnd, &m_WindowRect);

			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

			HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo{};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(m_hWnd, HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		}
		else
		{
			::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
				m_WindowRect.left,
				m_WindowRect.top,
				m_WindowRect.right - m_WindowRect.left,
				m_WindowRect.bottom - m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWnd, SW_NORMAL);
		}
	}
}

void Window::ToggleFullScreen()
{
	SetFullScreen(!m_Fullscreen);
}

void Window::Show()
{
	::ShowWindow(m_hWnd, SW_SHOW);
}

void Window::Hide()
{
	::ShowWindow(m_hWnd, SW_HIDE);
}

UINT Window::GetCurrentBackBufferIndex() const
{
	return m_CurrentBackBufferIndex;
}

UINT Window::Present()
{
	UINT syncInterval = m_VSync ? 1 : 0;
	UINT presentFlags = m_TearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(m_SwapChain->Present(syncInterval, presentFlags));
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return m_CurrentBackBufferIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE Window::GetCurrentRenderTargetView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_CurrentBackBufferIndex, m_RTVDescriptorSize);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Window::GetCurrentBackBuffer() const
{
	return m_BackBuffers[m_CurrentBackBufferIndex];
}
