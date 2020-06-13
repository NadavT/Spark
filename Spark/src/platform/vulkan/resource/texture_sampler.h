#pragma once

#include "resource.h"

#include "platform/vulkan/vulkan_context.h"

namespace Spark
{
	class VulkanTextureSampler : public VulkanResourse
	{
	public:
		VulkanTextureSampler(VulkanContext& context, const std::string& name);
		virtual ~VulkanTextureSampler();

		VkSampler getSampler() const;

	private:
		VulkanContext& m_context;
		VkSampler m_textureSampler;
	};
}

