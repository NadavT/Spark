#pragma once

#include "spark/core/key_codes.h"
#include "spark/event/event.h"


namespace Spark
{

class KeyEvent : public Event
{
  public:
    inline KeyCode GetKeyCode() const
    {
        return m_KeyCode;
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": keycode=" << m_KeyCode;
        return stream.str();
    }

    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
  protected:
    KeyEvent(KeyCode keycode)
        : m_KeyCode(keycode)
    {
    }

    KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent
{
  public:
    KeyPressedEvent(KeyCode keycode, int repeatCount)
        : KeyEvent(keycode)
        , m_RepeatCount(repeatCount)
    {
    }

    inline int GetRepeatCount() const
    {
        return m_RepeatCount;
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << Event::ToString() << ": keycode=" << m_KeyCode << " (" << m_RepeatCount << " repeats)";
        return stream.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
  private:
    int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
  public:
    KeyReleasedEvent(KeyCode keycode)
        : KeyEvent(keycode)
    {
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << "KeyReleasedEvent: " << m_KeyCode;
        return stream.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent
{
  public:
    KeyTypedEvent(KeyCode keycode)
        : KeyEvent(keycode)
    {
    }

    std::string ToString() const override
    {
        std::stringstream stream;
        stream << "KeyTypedEvent: " << m_KeyCode;
        return stream.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};
} // namespace Spark