#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline2d.h"
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
		virtual void OnUpdate(Time& diffTime);
		virtual void OnRender();

	private:
		void createCommandBuffers();
	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;
		VulkanPipeline2D* m_pipeline;

		std::unique_ptr<Quad> m_quad;
		std::vector<VkBuffer> m_uniformTransformations;
		std::vector<VkDeviceMemory> m_uniformTransformationsMemory;
		std::vector<VkDescriptorSet> m_transfomationDescriptorSets;

		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}
