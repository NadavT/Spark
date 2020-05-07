#pragma once

#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/framebuffer/framebuffer.h"

namespace Spark
{
	enum class VulkanPipelineType
	{
		Type2D,
		Type3D
	};

	class VulkanPipeline
	{
	public:
		virtual ~VulkanPipeline();
		VulkanPipeline(const VulkanPipeline& other) = delete;
		VulkanPipeline& operator=(const VulkanPipeline& other) = delete;

		virtual void cleanup();
		virtual void recreate();
	protected:
		VulkanPipeline(VulkanContext& context, VulkanFramebuffer& framebuffer);

	protected:
		VulkanContext& m_context;
		VulkanFramebuffer& m_framebuffer;
	};
}
