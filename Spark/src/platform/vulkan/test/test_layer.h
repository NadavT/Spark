#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/test/quad.h"

namespace Spark
{
	class VulkanTestLayer : public Layer
	{
	public:
		VulkanTestLayer(VulkanRenderer& renderer);
		virtual ~VulkanTestLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnRender();

	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;
		VulkanPipeline* m_pipeline;

		std::vector<VkCommandBuffer> m_commandBuffers;

		std::unique_ptr<Quad> m_quad;
	};
}
