#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline.h"

namespace Spark
{
	class VulkanTriangleLayer : public Layer
	{
	public:
		VulkanTriangleLayer(VulkanRenderer& renderer);
		virtual ~VulkanTriangleLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnRender();

	private:
		void createCommandBuffers();
	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;
		VulkanPipeline* m_pipeline;

		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}
