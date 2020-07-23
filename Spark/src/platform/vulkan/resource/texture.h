#pragma once

#include "spark/resource/texture.h"

#include "platform/vulkan/vulkan_context.h"

namespace Spark
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(VulkanContext& context, const std::string& name, const std::string& path);
		virtual ~VulkanTexture();

		VkImageView getImageView() const;

	private:
		VulkanContext& m_context;
		VkImage m_textureImage;
		VkDeviceMemory m_textureImageMemory;
		VkImageView m_textureImageView;
	};
}
