#include "texture_image.h"

#include "spark/core/log.h"

#include <stb_image.h>

namespace Spark
{
	VulkanTextureImage::VulkanTextureImage(VulkanContext& context, const std::string& name, const std::string& path)
		: Resource(name, ResourceType::internal)
		, m_context(context)
		, m_textureImage(VK_NULL_HANDLE)
		, m_textureImageMemory(VK_NULL_HANDLE)
        , m_textureImageView(VK_NULL_HANDLE)
	{
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        SPARK_CORE_ASSERT(pixels, "failed to load texture image!");

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        m_context.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(m_context.m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_context.m_device, stagingBufferMemory);

        stbi_image_free(pixels);

        m_context.createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

        m_context.transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_context.copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        m_context.transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_context.m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_context.m_device, stagingBufferMemory, nullptr);

        m_textureImageView = m_context.createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
	
	VulkanTextureImage::~VulkanTextureImage()
	{
        vkDestroyImageView(m_context.m_device, m_textureImageView, nullptr);

        vkDestroyImage(m_context.m_device, m_textureImage, nullptr);
        vkFreeMemory(m_context.m_device, m_textureImageMemory, nullptr);
	}

    VkImageView VulkanTextureImage::getImageView() const
    {
        return m_textureImageView;
    }
}
