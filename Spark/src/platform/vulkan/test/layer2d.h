#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline2d.h"
#include "platform/vulkan/drawables/quad.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"
#include "platform/vulkan/layers/layer_renderer_2d.h"

#include "spark/event/keyboard_event.h"

namespace Spark
{
	class VulkanLayer2D : public Layer
	{
	public:
		VulkanLayer2D(VulkanRenderer& renderer);
		virtual ~VulkanLayer2D();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(Time& diffTime);
		virtual void OnEvent(Event& e);
		virtual void OnRender();

	private:
		bool handleKeyPressed(KeyPressedEvent& e);
	private:
		VulkanRenderer& m_renderer;
		VulkanLayerRenderer2D m_layer_renderer;

		std::unique_ptr<VulkanQuad> m_quad;
		std::unique_ptr<VulkanQuad> m_quad2;

		std::unique_ptr<VulkanTexture> m_texture;
		std::unique_ptr<VulkanTextureSampler> m_sampler;
	};
}
