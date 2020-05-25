#include "pipeline.h"

namespace Spark
{
	VulkanPipeline::VulkanPipeline(VulkanContext& context, VulkanFramebuffer& framebuffer)
		: m_context(context)
		, m_framebuffer(framebuffer)
	{

	}

	VulkanPipeline::~VulkanPipeline()
	{
		cleanup();
	}

	void VulkanPipeline::cleanup()
	{
		vkDestroyPipeline(m_context.m_device, m_pipeline, nullptr);
		vkDestroyPipelineLayout(m_context.m_device, m_pipelineLayout, nullptr);
	}

	void VulkanPipeline::recreate()
	{
		m_pipelineLayout = VK_NULL_HANDLE;
		m_pipeline = VK_NULL_HANDLE;
	}

	VkPipelineLayout VulkanPipeline::getLayout()
	{
		return m_pipelineLayout;
	}
}
