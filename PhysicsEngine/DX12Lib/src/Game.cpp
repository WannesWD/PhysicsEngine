#include "PCH.h"
#include "Game.h"

#include <string>
#include <Window.h>

Game::Game(const std::wstring& name, int width, int height, bool vSync) :
	m_Name{ name },
	m_Width{ width },
	m_Height{ height },
	m_vSync{ vSync }
{
}

Game::~Game()
{
}

bool Game::Initialize()
{
	if (!DirectX::XMVerifyCPUSupport())
	{
		MessageBox(NULL, "Failed to verify DirectX math library support", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_WindowPtr = App::Get().CreateRenderWindow(m_Name, m_Width, m_Height, m_vSync);
	m_WindowPtr->RegisterCallbacks(this);
	m_WindowPtr->Show();
}

void Game::Destroy()
{
	App::Get().DestroyWindow(m_WindowPtr);
	m_WindowPtr = nullptr;
}

void Game::OnUpdate(UpdateEventArgs& e)
{
}

void Game::OnRender(RenderEventArgs& e)
{
}

void Game::OnResize(ResizeEventArgs& e)
{
	m_Width = e.Width;
	m_Height = e.Height;
}

void Game::OnWindowDestroy()
{
	m_WindowPtr = nullptr;
}
