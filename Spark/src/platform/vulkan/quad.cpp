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

	Quad::Quad(VulkanContext& context, glm::vec2 position)
		: m_context(context)
		, m_model()
		, m_vertexBuffer(VK_NULL_HANDLE)
		, m_vertexBufferMemory(VK_NULL_HANDLE)
		, m_verticesOffset(0)
		, m_indicesOffset(0)
	{
		m_model = glm::translate(glm::mat3(1.0), position);

		createVertex2DBuffer(context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, quad_vertices, quad_indices);
	}

	Quad::~Quad()
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

	Quad::Quad(const Quad& other)
		: m_context(other.m_context)
	{
		copyQuad(other);
	}

	Quad::Quad(Quad&& other) noexcept
		: m_context(other.m_context)
	{
		moveQuad(other);
	}

	Quad& Quad::operator=(const Quad& other)
	{
		copyQuad(other);
		return *this;
	}

	Quad& Quad::operator=(Quad&& other) noexcept
	{
		moveQuad(other);
		return *this;
	}

	void Quad::fillCommandBuffer(VkCommandBuffer commandBuffer)
	{
		VkBuffer buff[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { m_verticesOffset };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);
	}

	glm::mat3 Quad::getModelMatrix()
	{
		return m_model;
	}

	void Quad::copyQuad(const Quad& other)
	{
		m_model = other.m_model;
		createVertex2DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, quad_vertices, quad_indices);
	}

	void Quad::moveQuad(Quad& other) noexcept
	{
		m_model = other.m_model;
		m_vertexBuffer = other.m_vertexBuffer;
		other.m_vertexBuffer = VK_NULL_HANDLE;
		m_vertexBufferMemory = other.m_vertexBufferMemory;
		other.m_vertexBufferMemory = VK_NULL_HANDLE;
		m_verticesOffset = other.m_verticesOffset;
		m_indicesOffset = other.m_indicesOffset;
	}
}
