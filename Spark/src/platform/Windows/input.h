#ifndef SPARK_WINDOWS_INPUT_H
#define SPARK_WINDOWS_INPUT_H

#include "spark/core/input.h"

#include "window.h"

namespace Spark
{
class WindowsInput : public Input
{
  public:
    WindowsInput(const WindowsWindow &window);

  protected:
    virtual bool IsKeyPressedImpl(KeyCode key);

    virtual bool IsMouseButtonPressedImpl(MouseCode button);
    virtual glm::vec2 GetMousePositionImpl();
    virtual float GetMouseXImpl();
    virtual float GetMouseYImpl();
    virtual void HideMouseImpl();
    virtual void UnHideMouseImpl();

  private:
    const WindowsWindow &m_window;
};
} // namespace Spark

#endif /* SPARK_WINDOWS_INPUT_H */
