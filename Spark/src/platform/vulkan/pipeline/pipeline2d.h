#pragma once

#include "platform/vulkan/pipeline/pipeline.h"

namespace Spark
{
	class VulkanPipeline2D : public VulkanPipeline
	{
	public:
		VulkanPipeline2D(VulkanContext& context, VulkanFramebuffer& framebuffer);
		virtual ~VulkanPipeline2D();

		virtual void cleanup();
		virtual void recreate();

		virtual void bind(VkCommandBuffer commandBuffer);

		VkDescriptorSetLayout getMVPDescriptorSetLayout();
		VkDescriptorSetLayout getTextureDescriptorSetLayout();

	private:
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
	private:
		VkDescriptorSetLayout m_MVPDescriptorSetLayout;
		VkDescriptorSetLayout m_textureDescriptorSetLayout;
	};
}
