#include "sphere.h"

#include "platform/vulkan/renderer.h"

#include "spark/core/application.h"

namespace Spark
{

VulkanSphere::VulkanSphere(VulkanRenderer &renderer, glm::vec3 position, int sectors, int stacks, glm::vec3 scale)
    : Sphere(position, sectors, stacks, scale)
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

VulkanSphere::~VulkanSphere()
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

VulkanSphere::VulkanSphere(const VulkanSphere &other)
    : Sphere(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
{
    copySphere(other);
}

VulkanSphere::VulkanSphere(VulkanSphere &&other) noexcept
    : Sphere(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
{
    moveSphere(other);
}

VulkanSphere &VulkanSphere::operator=(const VulkanSphere &other)
{
    copySphere(other);
    return *this;
}

VulkanSphere &VulkanSphere::operator=(VulkanSphere &&other) noexcept
{
    moveSphere(other);
    return *this;
}

void VulkanSphere::fillCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void VulkanSphere::fillVeticesAndIndices()
{
    float radius = 0.5f;

    float x, y, z, xy;               // vertex position
    float nx, ny, nz;                // normals
    float lengthInv = 1.0f / radius; // Inverted length
    float s, t;                      // texCoord

    float sectorStep = 2 * glm::pi<float>() / m_sectors;
    float stackStep = glm::pi<float>() / m_stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= m_stacks; ++i)
    {
        stackAngle = glm::pi<float>() / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xy = radius * glm::cos(stackAngle);                // r * cos(u)
        z = radius * glm::sin(stackAngle);                 // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= m_sectors; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position
            x = xy * glm::cos(sectorAngle); // r * cos(u) * cos(v)
            y = xy * glm::sin(sectorAngle); // r * cos(u) * sin(v)

            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // vertex tex coord between [0, 1]
            s = (float)j / m_sectors;
            t = (float)i / m_stacks;

            m_vertices.push_back({{x, y, z}, {s, t}, {nx, ny, nz}});
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < m_stacks; ++i)
    {
        k1 = i * (m_sectors + 1); // beginning of current stack
        k2 = k1 + m_sectors + 1;  // beginning of next stack

        for (int j = 0; j < m_sectors; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                m_indices.push_back(k1);     // k1---k2---k1+1
                m_indices.push_back(k2);     // k1---k2---k1+1
                m_indices.push_back(k1 + 1); // k1---k2---k1+1
            }

            if (i != (m_stacks - 1))
            {
                m_indices.push_back(k1 + 1); // k1+1---k2---k2+1
                m_indices.push_back(k2);     // k1+1---k2---k2+1
                m_indices.push_back(k2 + 1); // k1+1---k2---k2+1
            }
        }
    }
}

void VulkanSphere::copySphere(const VulkanSphere &other)
{
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, m_vertices,
                         m_indices);
}
void VulkanSphere::moveSphere(VulkanSphere &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark
