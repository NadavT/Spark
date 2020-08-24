#include <spark.h>
#include <spark/core/entry_point.h>

class SandboxLayer : public Spark::Layer2D
{
public:
	SandboxLayer()
		:Spark::Layer2D("Sandbox layer")
		, counter(0)
	{
		const Spark::Texture& texture = Spark::ResourceManager::loadTexture("sandboxTexture", "C:\\Users\\nadav\\OneDrive\\Pictures\\Capture.PNG");
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
