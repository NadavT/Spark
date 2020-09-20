#include "input.h"

#ifdef SPARK_PLATFORM_WINDOWS
    #include "platform/Windows/input.h"
#endif

namespace Spark
{
std::unique_ptr<Input> s_input = nullptr;
bool s_blockInput = false;

void Input::Init(const Application &app)
{
#ifdef SPARK_PLATFORM_WINDOWS
    s_input = std::make_unique<WindowsInput>(reinterpret_cast<const WindowsWindow &>(app.GetWindow()));
#endif
}

void Input::Destroy()
{
    s_input.reset(nullptr);
}

bool Input::IsKeyPressed(KeyCode key)
{
    return !s_blockInput && s_input->IsKeyPressedImpl(key);
}

bool Input::IsMouseButtonPressed(MouseCode button)
{
    return !s_blockInput && s_input->IsMouseButtonPressedImpl(button);
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

void Input::BlockInput()
{
    s_blockInput = true;
}

void Input::UnBlockInput()
{
    s_blockInput = false;
}

void Input::HideMouse()
{
    s_input->HideMouseImpl();
}

void Input::UnHideMouse()
{
    s_input->UnHideMouseImpl();
}
} // namespace Spark
