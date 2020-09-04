#include <spark.h>
#include <spark/core/entry_point.h>

class SandboxLayer : public Spark::Layer2D
{
public:
	SandboxLayer()
		:Spark::Layer2D("Sandbox layer")
		, counter(0)
	{
		const Spark::Texture& texture = Spark::ResourceManager::loadTexture("sandboxTexture", "C:\\Users\\nadav\\source\\repos\\vulkan-tutorial\\VulkanTutorial\\textures\\container2.png");
		Spark::ResourceManager::loadTexture("sandboxTexture2", "C:\\Users\\nadav\\OneDrive\\Pictures\\ophir.PNG");
		m_quads.push_back(std::move(Spark::createQuad(glm::vec2(0, 0), texture, glm::vec2(1))));
		addDrawable(m_quads.back());
	}

	virtual void OnAttach()
	{
		SPARK_INFO("Attached {0}", getName());
		Layer2D::OnAttach();
	}

	virtual void OnDetach()
	{
		Layer2D::OnDetach();
		SPARK_INFO("Detached {0}", getName());
	}

	virtual void OnUpdate(Spark::Time& diffTime)
	{
		Layer2D::OnUpdate(diffTime);
	}

	bool handleKeyPressed(Spark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Spark::KeyCode::Left:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->move({-0.05, 0});
			return true;
		case Spark::KeyCode::Right:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->move({0.05, 0});
			return true;
		case Spark::KeyCode::Down:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->move({0, 0.05});
			return true;
		case Spark::KeyCode::Up:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->move({0, -0.05});
			return true;
		case Spark::KeyCode::A:
			counter++;
			m_quads.push_back(std::move(Spark::createQuad(glm::vec2(-0.5 + counter/10.0, -0.5), *Spark::ResourceManager::getTexture("sandboxTexture2"))));
			addDrawable(m_quads.back());
			return true;
		case Spark::KeyCode::D:
			removeDrawable(m_quads.back().get());
			m_quads.pop_back();
			return true;
		case Spark::KeyCode::Z:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(0.95f));
			return true;
		case Spark::KeyCode::X:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(1.05f));
			return true;
		case Spark::KeyCode::Q:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(0.95f, 1));
			return true;
		case Spark::KeyCode::W:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(1.05f, 1));
			return true;
		case Spark::KeyCode::E:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(1, 0.95f));
			return true;
		case Spark::KeyCode::R:
			reinterpret_cast<Spark::Quad*>(m_quads.front().get())->scale(glm::vec2(1, 1.05f));
			return true;
		default:
			return false;
		}
	}

	virtual void OnEvent(Spark::Event& e)
	{
		Layer2D::OnEvent(e);
		Spark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(SandboxLayer::handleKeyPressed));
	}


private:
	int counter;
	std::vector<std::shared_ptr<Spark::Drawable>> m_quads;
};

class Sandbox3DLayer : public Spark::Layer3D
{
public:
	Sandbox3DLayer()
		: m_camera({10.0f, 0.0f, 0.0f})
		, Spark::Layer3D("Sandbox layer", m_camera)
		, m_focused(false)
	{
		const Spark::Texture& texture = Spark::ResourceManager::loadTexture("cubeTexutre", "C:\\Users\\nadav\\source\\repos\\vulkan-tutorial\\VulkanTutorial\\textures\\container2.png");
		m_cube = std::move(Spark::createCube({0, 0, 0}, texture));
		addDrawable(m_cube);
	}

	virtual void OnAttach()
	{
		SPARK_INFO("Attached {0}", getName());
		Layer3D::OnAttach();
	}

	virtual void OnDetach()
	{
		Spark::Input::UnHideMouse();
		Layer3D::OnDetach();
		SPARK_INFO("Detached {0}", getName());
	}

	virtual void OnUpdate(Spark::Time& diffTime)
	{
		Layer3D::OnUpdate(diffTime);

		if (!m_focused) {
			return;
		}

		if (Spark::Input::IsKeyPressed(Spark::KeyCode::A)) {
			m_camera.moveDirection(Spark::CameraDirection::LEFT, diffTime);
		}
		if (Spark::Input::IsKeyPressed(Spark::KeyCode::D)) {
			m_camera.moveDirection(Spark::CameraDirection::RIGHT, diffTime);
		}
		if (Spark::Input::IsKeyPressed(Spark::KeyCode::W)) {
			m_camera.moveDirection(Spark::CameraDirection::FORWARD, diffTime);
		}
		if (Spark::Input::IsKeyPressed(Spark::KeyCode::S)) {
			m_camera.moveDirection(Spark::CameraDirection::BACKWARD, diffTime);
		}
		if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftShift)) {
			m_camera.moveDirection(Spark::CameraDirection::UP, diffTime);
		}
		if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftControl)) {
			m_camera.moveDirection(Spark::CameraDirection::DOWN, diffTime);
		}
	}

	virtual void OnEvent(Spark::Event& e)
	{
		Layer3D::OnEvent(e);
		Spark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMouseMoved));
		dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleKeyPressed));
		dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMousePressed));
	}

	bool handleMouseMoved(Spark::MouseMovedEvent& e)
	{
		if (!m_focused) {
			return false;
		}

		m_camera.moveAngle(e.GetDiffX(), e.GetDiffY());
		return true;
	}

	bool handleKeyPressed(Spark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Spark::KeyCode::Escape:
			Spark::Input::UnHideMouse();
			m_focused = false;
			return true;
		default:
			return false;
		}
	}
				
	bool handleMousePressed(Spark::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Spark::MouseCode::ButtonLeft:
			Spark::Input::HideMouse();
			m_focused = true;
			return true;
		default:
			return false;
		}
	}

private:
	std::shared_ptr<Spark::Drawable> m_cube;
	Spark::Camera m_camera;
	bool m_focused;
};

class Sandbox : public Spark::Application
{
public:
	Sandbox()
		: layer()
	{
		layer = std::make_unique<Sandbox3DLayer>();
		PushLayer(layer.get());
	}

	bool handleKeyPressed(Spark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		// case Spark::KeyCode::S:
		// 	PopLayer(layer.get());
		// 	layer.reset(nullptr);
		// 	return true;
		default:
			return false;
		}
	}

	virtual void handleEvent(Spark::Event& e)
	{
		Application::handleEvent(e);
		Spark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox::handleKeyPressed));
	}

	~Sandbox()
	{
		if (layer.get() != nullptr)
		{
			PopLayer(layer.get());
		}
	}

private:
	std::unique_ptr<Sandbox3DLayer> layer;
};

Spark::Application* Spark::CreateApplication()
{
	return new Sandbox();
}
