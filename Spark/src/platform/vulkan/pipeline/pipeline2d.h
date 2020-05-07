#pragma once

#include "platform/vulkan/pipeline/pipeline.h"

namespace Spark
{
	class VulkanPipeline2D : public VulkanPipeline
	{
		VulkanPipeline2D(VulkanContext& context, VulkanFramebuffer& framebuffer);
		virtual ~VulkanPipeline2D();

		virtual void cleanup();
		virtual void recreate();

	private:

	private:

	};
}
