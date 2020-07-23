#include "layer2d.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

namespace Spark
{
	VulkanLayer2D::VulkanLayer2D(VulkanRenderer& renderer)
		: Layer("2d layer")
		, m_renderer(renderer)
		, m_quad(nullptr)
		, m_texture(nullptr)
		, m_sampler(nullptr)
		, m_layer_renderer(renderer)
	{
		m_texture = std::make_unique<VulkanTexture>(renderer.m_context, "texture", "C:\\Users\\nadav\\onedrive\\Pictures\\Capture.png");
		m_sampler = std::make_unique<VulkanTextureSampler>(renderer.m_context, "sampler");
		m_quad = std::make_unique<VulkanQuad>(m_renderer.m_context, glm::vec2(-0.5, -0.5), *m_texture, *m_sampler);
		m_quad2 = std::make_unique<VulkanQuad>(m_renderer.m_context, glm::vec2(0.5, 0.5), *m_texture, *m_sampler);
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
