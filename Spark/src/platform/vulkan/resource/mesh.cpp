#include "mesh.h"

#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
VulkanMesh::VulkanMesh(VulkanRenderer &renderer, std::vector<Vertex3D> vertices, std::vector<unsigned int> indices,
                       std::vector<const Texture *> &textures)
    : Mesh(vertices, indices, textures)
    , m_renderer(renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
{
    createVertex3DBuffer(m_renderer.getContext(), m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset,
                         m_indicesOffset, m_vertices, m_indices);
}

VulkanMesh::~VulkanMesh()
{
    if (m_vertexBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_renderer.m_context.m_device, m_vertexBuffer, nullptr);
    }
    if (m_vertexBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_renderer.m_context.m_device, m_vertexBufferMemory, nullptr);
    }
}

VulkanMesh::VulkanMesh(const VulkanMesh &other)
    : Mesh(other)
    , m_renderer(m_renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
{
    copyMesh(other);
}

VulkanMesh::VulkanMesh(VulkanMesh &&other) noexcept
    : Mesh(other)
    , m_renderer(m_renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
{
    moveMesh(other);
}

VulkanMesh &VulkanMesh::operator=(const VulkanMesh &other)
{
    copyMesh(other);
    return *this;
}

VulkanMesh &VulkanMesh::operator=(VulkanMesh &&other) noexcept
{
    moveMesh(other);
    return *this;
}

VulkanDrawableType VulkanMesh::getDrawableType() const
{
    return VulkanDrawableType::Mesh;
}

void VulkanMesh::fillCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void VulkanMesh::copyMesh(const VulkanMesh &other)
{
    createVertex3DBuffer(m_renderer.getContext(), m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset,
                         m_indicesOffset, m_vertices, m_indices);
}

void VulkanMesh::moveMesh(VulkanMesh &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark::Render
