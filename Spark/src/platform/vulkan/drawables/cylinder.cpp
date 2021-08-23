#include "cylinder.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark::Render
{

VulkanCylinder::VulkanCylinder(VulkanRenderer &renderer, glm::vec3 position, float baseRadius, float topRadius,
                               float height, int sectors, int stacks, glm::vec3 scale)
    : Drawable3D(position, scale)
    , Cylinder(position, baseRadius, topRadius, height, sectors, stacks, scale)
    , m_context(renderer.m_context)
    , m_renderer(renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
    , m_vertices()
    , m_indices()
{
    fillVeticesAndIndices();
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, m_vertices,
                         m_indices);
}

VulkanCylinder::~VulkanCylinder()
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

VulkanCylinder::VulkanCylinder(const VulkanCylinder &other)
    : Drawable3D(other)
    , Cylinder(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
{
    copyCylinder(other);
}

VulkanCylinder::VulkanCylinder(VulkanCylinder &&other) noexcept
    : Drawable3D(other)
    , Cylinder(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
{
    moveCylinder(other);
}

VulkanCylinder &VulkanCylinder::operator=(const VulkanCylinder &other)
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
        copyCylinder(other);
    }
    return *this;
}

VulkanCylinder &VulkanCylinder::operator=(VulkanCylinder &&other) noexcept
{
    if (this != &other)
    {
        moveCylinder(other);
    }
    return *this;
}

void VulkanCylinder::fillCommandBuffer(VkCommandBuffer commandBuffer) const
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void VulkanCylinder::fillVeticesAndIndices()
{
    float radius;

    float x, y, z;    // vertex position
    float nx, ny, nz; // normals
    float s, t;       // texCoord

    float sectorStep = 2 * glm::pi<float>() / m_sectors;
    float sectorAngle;

    // compute the normal vector at 0 degree first
    // tanA = (baseRadius-topRadius) / height
    float zAngle = glm::atan((m_baseRadius - m_topRadius) / 2, m_height);
    float nx0 = glm::cos(zAngle);
    float ny0 = 0;
    float nz0 = glm::sin(zAngle);

    // Create base and top center vertices
    m_vertices.push_back({{0, 0, m_height * -0.5f}, {0.5f, 0.5f}, {0, 0, -1}});
    m_vertices.push_back({{0, 0, m_height * 0.5f}, {0.5f, 0.5f}, {0, 0, 1}});

    for (int i = 0; i <= m_stacks; ++i)
    {
        // Get current stack radius
        radius = (m_baseRadius / 2) + (float)i / m_stacks * ((m_topRadius - m_baseRadius) / 2);
        z = -(m_height * 0.5f) + (float)i / m_stacks * m_height; // vertex position z

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= m_sectors; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position
            x = radius * glm::cos(sectorAngle);
            y = radius * glm::sin(sectorAngle);

            // normalized vertex normal
            nx = glm::cos(sectorAngle) * nx0 - glm::sin(sectorAngle) * ny0;
            ny = glm::sin(sectorAngle) * nx0 + glm::cos(sectorAngle) * ny0;
            nz = nz0;

            // vertex tex coord between [0, 1]
            s = (float)j / m_sectors;
            t = 1.0f - (float)i / m_stacks; // We are going from bottom to top

            m_vertices.push_back({{x, y, z}, {s, t}, {nx, ny, nz}});
        }
    }

    // Indices for base
    for (int i = 0; i < m_sectors; ++i)
    {
        if (i < (m_sectors - 1))
        {
            m_indices.push_back(0);
            m_indices.push_back(i + 3);
            m_indices.push_back(i + 2);
        }
        else
        {
            m_indices.push_back(0);
            m_indices.push_back(2);
            m_indices.push_back(i + 2);
        }
    }
    // Indices for top
    for (int i = 0; i < m_sectors; ++i)
    {
        if (i < (m_sectors - 1))
        {
            m_indices.push_back(1);
            m_indices.push_back(static_cast<uint32_t>(m_vertices.size()) - 2 - i);
            m_indices.push_back(static_cast<uint32_t>(m_vertices.size()) - 1 - i);
        }
        else
        {
            m_indices.push_back(1);
            m_indices.push_back(static_cast<uint32_t>(m_vertices.size()) - 2);
            m_indices.push_back(static_cast<uint32_t>(m_vertices.size()) - 1 - i);
        }
    }
    // Indices for sides
    unsigned int k1, k2;
    for (int i = 0; i < m_stacks; ++i)
    {
        k1 = 2 + i * (m_sectors + 1); // bebinning of current stack
        k2 = k1 + m_sectors + 1;      // beginning of next stack

        for (int j = 0; j < m_sectors; ++j, ++k1, ++k2)
        {
            // 2 trianles per sector
            m_indices.push_back(k1);
            m_indices.push_back(k1 + 1);
            m_indices.push_back(k2);

            m_indices.push_back(k2);
            m_indices.push_back(k1 + 1);
            m_indices.push_back(k2 + 1);
        }
    }
}

void VulkanCylinder::copyCylinder(const VulkanCylinder &other)
{
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, m_vertices,
                         m_indices);
}
void VulkanCylinder::moveCylinder(VulkanCylinder &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark::Render
