#pragma once
#include "platform/vulkan/vertex/vertex2d.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/resource/texture.h"
#include "platform/vulkan/resource/texture_sampler.h"

#include "spark/renderer/drawables/quad.h"

namespace Spark
{
	class VulkanQuad : public Quad
	{
	public:
		VulkanQuad(VulkanRenderer& renderer, glm::vec2 position, const VulkanTexture& texture, glm::vec2 scale = glm::vec2(1.0));
		virtual ~VulkanQuad();
		VulkanQuad(const VulkanQuad& other);
		VulkanQuad(VulkanQuad&& other) noexcept;
		VulkanQuad& operator=(const VulkanQuad& other);
		VulkanQuad& operator=(VulkanQuad&& other) noexcept;

		void fillCommandBuffer(VkCommandBuffer commandBuffer);

		virtual const VulkanTexture& getTexture() const;

	private:
		void copyQuad(const VulkanQuad& other);
		void moveQuad(VulkanQuad& other) noexcept;

	private:
		VulkanContext& m_context;
		VulkanRenderer& m_renderer;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkDeviceSize m_verticesOffset;
		VkDeviceSize m_indicesOffset;
		const VulkanTexture& m_texture;
	};
}
