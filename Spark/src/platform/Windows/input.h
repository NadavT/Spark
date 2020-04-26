#pragma once

#include "spark/core/input.h"

#include "window.h"

namespace Spark
{
	class WindowsInput : public Input
	{
	public:
		WindowsInput(const WindowsWindow& window);
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key);

		virtual bool IsMouseButtonPressedImpl(MouseCode button);
		virtual std::pair<float, float> GetMousePositionImpl();
		virtual float GetMouseXImpl();
		virtual float GetMouseYImpl();
	private:
		const WindowsWindow& m_window;
	};
}
