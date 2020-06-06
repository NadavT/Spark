#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/test/quad.h"

namespace Spark
{
	class VulkanLayer2D : public Layer
	{
	public:
		VulkanLayer2D(VulkanRenderer& renderer);
		virtual ~VulkanLayer2D();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnRender();

	private:
		void createCommandBuffers();
	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;
		VulkanPipeline* m_pipeline;

		std::unique_ptr<Quad> m_quad;

		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}
