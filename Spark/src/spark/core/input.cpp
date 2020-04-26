#include "input.h"

#ifdef SPARK_PLATFORM_WINDOWS
#include "platform/Windows/input.h"
#endif

namespace Spark
{
	std::unique_ptr<Input> s_input = nullptr;

	void Input::Init(const Application& app)
	{
		#ifdef SPARK_PLATFORM_WINDOWS
		s_input = std::make_unique<WindowsInput>(reinterpret_cast<const WindowsWindow&>(app.GetWindow()));
		#endif
	}

	bool Input::IsKeyPressed(KeyCode key)
	{
		return s_input->IsKeyPressedImpl(key);
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		return s_input->IsMouseButtonPressedImpl(button);
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		return s_input->GetMousePositionImpl();
	}

	float Input::GetMouseX()
	{
		return s_input->GetMouseXImpl();
	}

	float Input::GetMouseY()
	{
		return s_input->GetMouseYImpl();
	}
}
