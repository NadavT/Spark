#pragma once
#include "platform/vulkan/vertex/vertex2d.h"
#include "platform/vulkan/vulkan_context.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"

#include "spark/renderer/drawable.h"

namespace Spark
{
	class VulkanQuad : public Drawable
	{
	public:
		VulkanQuad(VulkanContext& context, glm::vec2 position, VulkanTexture& texture);
		virtual ~VulkanQuad();
		VulkanQuad(const VulkanQuad& other);
		VulkanQuad(VulkanQuad&& other) noexcept;
		VulkanQuad& operator=(const VulkanQuad& other);
		VulkanQuad& operator=(VulkanQuad&& other) noexcept;

		void fillCommandBuffer(VkCommandBuffer commandBuffer);
		virtual glm::mat3 getTransformation();

		virtual void move(glm::vec2 position);

		virtual VulkanTexture& getTexture() const;

	private:
		void copyQuad(const VulkanQuad& other);
		void moveQuad(VulkanQuad& other) noexcept;

	private:
		VulkanContext& m_context;
		glm::mat3 m_transformation;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkDeviceSize m_verticesOffset;
		VkDeviceSize m_indicesOffset;
		VulkanTexture& m_texture;
	};
}
