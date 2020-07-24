#include "quad.h"

namespace Spark
{
	const std::vector<Vertex2D> quad_vertices = {
		{{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
	};

	const std::vector<uint32_t> quad_indices = {
		0, 1, 2, 2, 3, 0,
	};

	VulkanQuad::VulkanQuad(VulkanContext& context, glm::vec2 position, const VulkanTexture& texture)
		: m_context(context)
		, m_transformation()
		, m_vertexBuffer(VK_NULL_HANDLE)
		, m_vertexBufferMemory(VK_NULL_HANDLE)
		, m_verticesOffset(0)
		, m_indicesOffset(0)
		, m_texture(texture)
	{
		m_transformation = glm::translate(glm::mat3(1.0), position);

		createVertex2DBuffer(context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, quad_vertices, quad_indices);
	}
	
	VulkanQuad::~VulkanQuad() 
	{
		if (m_vertexBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(m_context.m_device, m_vertexBuffer, nullptr);
		}
		if (m_vertexBufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_context.m_device, m_vertexBufferMemory, nullptr);
		}
	}
	
	VulkanQuad::VulkanQuad(const VulkanQuad& other)
		: m_context(other.m_context)
		, m_texture(other.m_texture)
	{
		copyQuad(other);
	}
	
	VulkanQuad::VulkanQuad(VulkanQuad&& other) noexcept
		: m_context(other.m_context)
		, m_texture(other.m_texture)
	{
		moveQuad(other);
	}
	
	VulkanQuad& VulkanQuad::operator=(const VulkanQuad& other) 
	{
		copyQuad(other);
		return *this;
	}
	
	VulkanQuad& VulkanQuad::operator=(VulkanQuad&& other) noexcept
	{
		moveQuad(other);
		return *this;
	}
	
	void VulkanQuad::fillCommandBuffer(VkCommandBuffer commandBuffer) 
	{
		VkBuffer buff[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { m_verticesOffset };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);
	}
	
	glm::mat3 VulkanQuad::getTransformation() 
	{
		return m_transformation;
	}
	
	void VulkanQuad::move(glm::vec2 position) 
	{
		m_transformation = glm::translate(m_transformation, position);
	}
	
	const VulkanTexture& VulkanQuad::getTexture() const
	{
		return m_texture;
	}
		
	void VulkanQuad::copyQuad(const VulkanQuad& other) 
	{
		m_transformation = other.m_transformation;
		createVertex2DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, quad_vertices, quad_indices);
	}
	
	void VulkanQuad::moveQuad(VulkanQuad& other) noexcept
	{
		m_transformation = other.m_transformation;
		m_vertexBuffer = other.m_vertexBuffer;
		other.m_vertexBuffer = VK_NULL_HANDLE;
		m_vertexBufferMemory = other.m_vertexBufferMemory;
		other.m_vertexBufferMemory = VK_NULL_HANDLE;
		m_verticesOffset = other.m_verticesOffset;
		m_indicesOffset = other.m_indicesOffset;
	}
}
