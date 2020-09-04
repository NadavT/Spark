#pragma once

#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark
{
	enum class VulkanPipelineType
	{
		Type2D,
		Type3D,
		TypeTriangle
	};

	class VulkanPipeline
	{
	public:
		virtual ~VulkanPipeline();
		VulkanPipeline(const VulkanPipeline& other) = delete;
		VulkanPipeline& operator=(const VulkanPipeline& other) = delete;

		virtual void cleanup();
		virtual void recreate();

		virtual void bind(VkCommandBuffer commandBuffer);

		VkPipelineLayout getLayout();
	protected:
		VulkanPipeline(VulkanContext& context, VulkanFramebuffer& framebuffer);
		VkShaderModule createShaderModule(const std::vector<char>& code);
		void createGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader, VkPipelineVertexInputStateCreateInfo vertexInputInfo, VkPipelineLayout layout, bool depthTesting = false);

	protected:
		VulkanContext& m_context;
		VulkanFramebuffer& m_framebuffer;
		VkPipeline m_pipeline;
	};
}
