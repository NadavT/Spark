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

	private:
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
		VkShaderModule createShaderModule(const std::vector<char>& code);
	private:
		VkDescriptorSetLayout m_MVPDescriptorSetLayout;
		VkDescriptorSetLayout m_textureDescriptorSetLayout;
	};
}
