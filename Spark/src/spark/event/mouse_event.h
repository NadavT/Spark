#ifndef SPARK_MOUSE_EVENT_H
#define SPARK_MOUSE_EVENT_H

#include "spark/event/event.h"

#include "spark/core/mouse_codes.h"

namespace Spark
{

class MouseMovedEvent : public Event
{
  public:
    MouseMovedEvent(float x, float y, float diffX, float diffY)
        : m_MouseX(x)
        , m_MouseY(y)
        , m_diffX(diffX)
        , m_diffY(diffY)
    {
    }

    float GetMouseX() const
    {
        return m_MouseX;
    }
    float GetMouseY() const
    {
        return m_MouseY;
    }
    float GetDiffX() const
    {
        return m_diffX;
    }
    float GetDiffY() const
    {
        return m_diffY;
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": " << m_MouseX << ", " << m_MouseY;
        return stream.str();
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
  private:
    float m_MouseX, m_MouseY, m_diffX, m_diffY;
};

class MouseScrolledEvent : public Event
{
  public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset)
        , m_YOffset(yOffset)
    {
    }

    float GetXOffset() const
    {
        return m_XOffset;
    }
    float GetYOffset() const
    {
        return m_YOffset;
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": " << GetXOffset() << ", " << GetYOffset();
        return stream.str();
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
  private:
    float m_XOffset, m_YOffset;
};

class MouseButtonEvent : public Event
{
  public:
    inline MouseCode GetMouseButton() const
    {
        return m_Button;
    }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
  protected:
    MouseButtonEvent(MouseCode button)
        : m_Button(button)
    {
    }

    MouseCode m_Button;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
  public:
    MouseButtonPressedEvent(MouseCode button)
        : MouseButtonEvent(button)
    {
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": " << m_Button;
        return stream.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
  public:
    MouseButtonReleasedEvent(MouseCode button)
        : MouseButtonEvent(button)
    {
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": " << m_Button;
        return stream.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace Spark

#endif /* SPARK_MOUSE_EVENT_H */
