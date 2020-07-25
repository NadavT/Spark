#include <spark.h>
#include <spark/core/entry_point.h>

class SandboxLayer : public Spark::Layer2D
{
public:
	SandboxLayer()
		:Spark::Layer2D("Sandbox layer")
	{
		const Spark::Texture& texture = Spark::ResourceManager::loadTexture("sandboxTexture", "C:\\Users\\NadavT\\Pictures\\poke.png");
		m_quad = std::move(Spark::createQuad(glm::vec2(0, 0), texture));
		addDrawable(m_quad.get());
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
			m_quad->move({-0.05, 0});
			return true;
		case Spark::KeyCode::Right:
			m_quad->move({0.05, 0});
			return true;
		case Spark::KeyCode::Down:
			m_quad->move({0, 0.05});
			return true;
		case Spark::KeyCode::Up:
			m_quad->move({0, -0.05});
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
	std::unique_ptr<Spark::Quad> m_quad;
};

class Sandbox : public Spark::Application
{
public:
	Sandbox()
		: layer()
	{
		PushLayer(&layer);
	}

	~Sandbox()
	{
		PopLayer(&layer);
	}

private:
	SandboxLayer layer;
};

Spark::Application* Spark::CreateApplication()
{
	return new Sandbox();
}
