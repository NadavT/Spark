#pragma once

#include "spark/core/core.h"
#include "spark/core/application.h"
#include "spark/core/window.h"
#include "spark/core/key_codes.h"
#include "spark/core/mouse_codes.h"

namespace Spark
{
	class Input
	{
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		SPARK_API static bool IsKeyPressed(KeyCode key);

		SPARK_API static bool IsMouseButtonPressed(MouseCode button);
		SPARK_API static std::pair<float, float> GetMousePosition();
		SPARK_API static float GetMouseX();
		SPARK_API static float GetMouseY();

		static void Init(const Application& app);
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;

		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	};
}
