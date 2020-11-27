#include "cube.h"

namespace Spark
{
const std::vector<Vertex3D> cube_vertices = {
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
};

const std::vector<uint32_t> cube_indices = {
    0,  1,  2,  2,  3,  0,  4,  7,  6,  6,  5,  4,  8,  9,  10, 10, 11, 8,
    12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
};

VulkanCube::VulkanCube(VulkanRenderer &renderer, glm::vec3 position, const VulkanTexture &texture,
                       const VulkanTexture &specularTexture, glm::vec3 scale)
    : Cube(position, scale)
    , m_context(renderer.m_context)
    , m_renderer(renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
    , m_texture(texture)
    , m_specularTexture(specularTexture)
{
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset,
                         cube_vertices, cube_indices);
}

VulkanCube::~VulkanCube()
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

VulkanCube::VulkanCube(const VulkanCube &other)
    : Cube(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_texture(other.m_texture)
    , m_specularTexture(other.m_specularTexture)
{
    copyCube(other);
}

VulkanCube::VulkanCube(VulkanCube &&other) noexcept
    : Cube(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_texture(other.m_texture)
    , m_specularTexture(other.m_specularTexture)
{
    moveCube(other);
}

VulkanCube &VulkanCube::operator=(const VulkanCube &other)
{
    copyCube(other);
    return *this;
}

VulkanCube &VulkanCube::operator=(VulkanCube &&other) noexcept
{
    moveCube(other);
    return *this;
}

void VulkanCube::fillCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
}

const VulkanTexture &VulkanCube::getTexture() const
{
    return m_texture;
}

const VulkanTexture &VulkanCube::getSpecularTexture() const
{
    return m_specularTexture;
}

void VulkanCube::copyCube(const VulkanCube &other)
{
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset,
                         cube_vertices, cube_indices);
}
void VulkanCube::moveCube(VulkanCube &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark
