#include "layer2d.h"

#include "spark/resource/resource_manager.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

namespace Spark
{
	VulkanLayer2D::VulkanLayer2D(VulkanRenderer& renderer)
		: Layer("2d layer")
		, m_renderer(renderer)
		, m_layer_renderer(renderer)
	{
		const VulkanTexture& texture0 = reinterpret_cast<const VulkanTexture&>(ResourceManager::loadTexture("texture0", "C:\\Users\\NadavT\\Pictures\\ball.png"));
		const VulkanTexture& texture1 = reinterpret_cast<const VulkanTexture&>(ResourceManager::loadTexture("texture1", "C:\\Users\\NadavT\\Pictures\\Untitled.png"));
		m_quad = std::make_unique<VulkanQuad>(m_renderer.m_context, glm::vec2(-0.5, -0.5), texture0);
		m_quad2 = std::make_unique<VulkanQuad>(m_renderer.m_context, glm::vec2(0.5, 0.5), texture1);
		m_layer_renderer.addDrawable(m_quad.get());
		m_layer_renderer.addDrawable(m_quad2.get());
	}

	VulkanLayer2D::~VulkanLayer2D()
	{
	}

	void VulkanLayer2D::OnAttach()
	{
		m_layer_renderer.OnAttach();
	}

	void VulkanLayer2D::OnDetach()
	{
		m_layer_renderer.OnDetach();
	}

	void VulkanLayer2D::OnUpdate(Time& diffTime)
	{
	}

	void VulkanLayer2D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SPARK_BIND_EVENT_FN(VulkanLayer2D::handleKeyPressed));
	}

	void VulkanLayer2D::OnRender()
	{
		m_layer_renderer.OnRender();
	}

	bool VulkanLayer2D::handleKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case KeyCode::Left:
			m_quad->move({-0.05, 0});
			return true;
		case KeyCode::Right:
			m_quad->move({0.05, 0});
			return true;
		case KeyCode::Down:
			m_quad->move({0, 0.05});
			return true;
		case KeyCode::Up:
			m_quad->move({0, -0.05});
			return true;
		default:
			return false;
		}
	}
}
