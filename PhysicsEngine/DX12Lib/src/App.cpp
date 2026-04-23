#include "PCH.h"
#include "App.h"
#include "..\resource.h"

#include "Game.h"
#include "CommandQueue.h"
#include "Window.h"

namespace Application
{
	constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

	using WindowPtr = std::unique_ptr<Window>;
	using WindowData = std::pair<HWND, WindowPtr>;
	using WindowMap = std::map<std::wstring, WindowData>;

	static std::unique_ptr<App> G_SINGLETON{ nullptr };
	static WindowMap G_WINDOWS{};


	void App::Create(HINSTANCE hInst)
	{
		if (!G_SINGLETON)
		{
			G_SINGLETON = std::make_unique<App>(hInst);
		}
	}

	void App::Destroy()
	{
		if (G_SINGLETON)
		{
			assert(G_WINDOWS.empty() && "All windows should be destroyed before destroying the application instance");

			G_SINGLETON.release();
			G_SINGLETON = nullptr;
		}
	}

	const App& App::Get()
	{
		assert(G_SINGLETON);
		return *G_SINGLETON;
	}

	bool App::IsTearingSupported() const
	{
		return m_TearingSupported;
	}

	Window* const App::CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
	{
		WindowMap::iterator windowIter = G_WINDOWS.find(windowName);
		if (windowIter != G_WINDOWS.end())
			return windowIter->second.second.get();

		RECT windowRect{ 0,0,clientWidth, clientHeight };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hWnd = CreateWindowW(WINDOW_CLASS_NAME, windowName.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr, m_hInstance, nullptr);

		if (!hWnd)
		{
			MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
			return nullptr;
		}

		WindowData data{ hWnd,std::make_unique<Window>(hWnd, windowName, clientWidth, clientHeight, vSync) };

		// emplacing because insert wont work with a unique pointer if i understand it correctly
		G_WINDOWS.emplace(windowName, data); 

		return data.second.get();
	}

	void App::DestroyWindow(const std::wstring& windowName)
	{
	}

	void App::DestroyWindow(Window* const window)
	{
	}

	Window* const App::GetWindowByName(const std::wstring& windowName)
	{
		return nullptr;
	}

	int App::Run(Game* const gamePtr)
	{
		return 0;
	}

	void App::Quit(int exitCode)
	{
	}

	Microsoft::WRL::ComPtr<ID3D12Device2> App::GetDevice() const
	{
		return Microsoft::WRL::ComPtr<ID3D12Device2>();
	}

	CommandQueue* const App::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		return nullptr;
	}

	void App::Flush()
	{
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> App::CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		return Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>();
	}

	UINT App::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
	{
		return 0;
	}

	App::App(HINSTANCE hInst) :
		m_hInstance{ hInst },
		m_TearingSupported{ false }
	{
		// Using this awareness context allows the client area of the window 
		// to achieve 100% scaling while still allowing non-client window content to 
		// be rendered in a DPI sensitive fashion.
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugInterface{};
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();
#endif // _DEBUG

		WNDCLASSEXW wndClass{ 0 };
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = &WndProc;
		wndClass.hInstance = m_hInstance;
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(APP_ICON));
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = WINDOW_CLASS_NAME;
		wndClass.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(APP_ICON));

		if (!RegisterClassExW(&wndClass))
		{
			MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
		}

		m_Adapter = GetAdapter(false);
		if (m_Adapter)
			m_Device = CreateDevice(m_Adapter);
		if (m_Device)
		{
			m_DirectCommandQueuePtr = std::make_unique<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			m_ComputeCommandQueuePtr = std::make_unique<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			m_CopyCommandQueuePtr = std::make_unique<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COPY);

			m_TearingSupported = CheckTearingSupport();
		}

	}

	App::~App()
	{
		Flush();
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> App::GetAdapter(bool bUseWarp)
	{
		ComPtr<IDXGIFactory4> dxgiFactory{};
		UINT createFactoryFlags{ 0 };

#ifdef _DEBUG
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		ComPtr<IDXGIAdapter1> adapter1{};
		ComPtr<IDXGIAdapter4> adapter4{};

		if (bUseWarp)
		{
			ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1)));
			ThrowIfFailed(adapter1.As(&adapter4));
		}
		else
		{
			SIZE_T maxDedicatedVideoMemory = 0;
			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
				adapter1->GetDesc1(&dxgiAdapterDesc1);

				if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
					SUCCEEDED(D3D12CreateDevice(adapter1.Get(),
						D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
					dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
				{
					maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
					ThrowIfFailed(adapter1.As(&adapter4));
				}
			}
		}

		return adapter4;
	}

	Microsoft::WRL::ComPtr<ID3D12Device2> App::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
	{
		ComPtr<ID3D12Device2> d3d12Device2{};
		ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

		// debug messages for debug mode
#if defined(_DEBUG)
		ComPtr<ID3D12InfoQueue> pInfoQueue{};
		if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif // defined(_DEBUG)

		return d3d12Device2;
	}

	bool App::CheckTearingSupport()
	{
		BOOL allowTearing = FALSE;

		ComPtr<IDXGIFactory4> factory4{};
		if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
		{
			ComPtr<IDXGIFactory5> factory5{};
			if (SUCCEEDED(factory4.As(&factory5)))
			{
				if (FAILED(factory5->CheckFeatureSupport(
					DXGI_FEATURE_PRESENT_ALLOW_TEARING,
					&allowTearing, sizeof(allowTearing))))
				{
					allowTearing = FALSE;
				}
			}
		}

		return allowTearing == TRUE;
	}
}


