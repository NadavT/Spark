#include "quad.h"

namespace Spark::Render
{
const std::vector<Vertex2D> quad_vertices = {
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
};

const std::vector<uint32_t> quad_indices = {
    0, 1, 2, 2, 3, 0,
};

VulkanQuad::VulkanQuad(VulkanRenderer &renderer, glm::vec2 position, const VulkanTexture &texture, glm::vec2 scale)
    : Drawable2D(position, scale)
    , Quad(position, scale)
    , m_context(renderer.m_context)
    , m_renderer(renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
    , m_texture(texture)
{
    createVertex2DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset,
                         quad_vertices, quad_indices);
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

VulkanQuad::VulkanQuad(const VulkanQuad &other)
    : Drawable2D(other)
    , Quad(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_texture(other.m_texture)
{
    copyQuad(other);
}

VulkanQuad::VulkanQuad(VulkanQuad &&other) noexcept
    : Drawable2D(other)
    , Quad(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_texture(other.m_texture)
{
    moveQuad(other);
}

VulkanQuad &VulkanQuad::operator=(const VulkanQuad &other)
{
    if (this != &other)
    {
        if (m_vertexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_context.m_device, m_vertexBuffer, nullptr);
        }
        if (m_vertexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(m_context.m_device, m_vertexBufferMemory, nullptr);
        }
        copyQuad(other);
    }
    return *this;
}

VulkanQuad &VulkanQuad::operator=(VulkanQuad &&other) noexcept
{
    if (this != &other)
    {
        moveQuad(other);
    }
    return *this;
}

void VulkanQuad::fillCommandBuffer(VkCommandBuffer commandBuffer) const
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);
}

VulkanDrawableType VulkanQuad::getDrawableType() const
{
    return VulkanDrawableType::Textured;
}

const VulkanTexture &VulkanQuad::getTexture() const
{
    return m_texture;
}

void VulkanQuad::copyQuad(const VulkanQuad &other)
{
    createVertex2DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset,
                         quad_vertices, quad_indices);
}

void VulkanQuad::moveQuad(VulkanQuad &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark::Render
