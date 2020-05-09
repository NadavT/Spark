#pragma once
#include "platform/vulkan/vertex/vertex2d.h"
#include "platform/vulkan/vulkan_context.h"

namespace Spark
{
	class Quad
	{
	public:
		Quad(VulkanContext& context, glm::vec2 position);
		~Quad();
		Quad(const Quad& other);
		Quad(Quad&& other) noexcept;
		Quad& operator=(const Quad& other);
		Quad& operator=(Quad&& other) noexcept;

		void fillCommandBuffer(VkCommandBuffer commandBuffer);
		glm::mat3 getModelMatrix();

	private:
		void copyQuad(const Quad& other);
		void moveQuad(Quad& other) noexcept;

		VulkanContext& m_context;
		glm::mat3 m_model;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkDeviceSize m_verticesOffset;
		VkDeviceSize m_indicesOffset;
	};
}
