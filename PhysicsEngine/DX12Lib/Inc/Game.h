#ifndef GAME_H
#define GAME_H
#endif


#include<Events.h>
#include<memory>


class Game
{
public:
	Game(const std::wstring& name, int width, int height, bool vSync);
	virtual ~Game();

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game& operator=(Game&& other) = delete;

	virtual bool Initialize();
	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;
	virtual void Destroy();

protected:
	friend class Window;
	virtual void OnUpdate(UpdateEventArgs& e);
	virtual void OnRender(RenderEventArgs& e);

	// handle inputs
	//virtual void OnKeyPressed(KeyEventArgs& e);
	//virtual void OnKeyReleased(KeyEventArgs& e);
	//virtual void OnMouseMoved(MouseMotionEventArg	s& e);
	//virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
	//virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
	//virtual void OnMouseWheel(MouseWheelEventArgs& e);
	virtual void OnResize(ResizeEventArgs& e);

	virtual void OnWindowDestroy();

	Window* const m_WindowPtr;

private:
	std::wstring m_Name{};
	int m_Width{};
	int m_Height{};
	bool m_vSync{};
};