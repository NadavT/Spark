#include "pipe.h"

#include "spark/core/log.h"

namespace Spark::Render
{
VulkanPipe::VulkanPipe(VulkanRenderer &renderer, std::vector<glm::vec3> positions, float radius, bool closed,
                       int sectors, glm::vec3 scale)
    : Pipe(positions, radius, closed, sectors, scale)
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

VulkanPipe::~VulkanPipe()
{
    vkDestroyBuffer(m_context.m_device, m_vertexBuffer, nullptr);
    vkFreeMemory(m_context.m_device, m_vertexBufferMemory, nullptr);
}

VulkanPipe::VulkanPipe(const VulkanPipe &other)
    : Pipe(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
    , m_vertices()
    , m_indices()
{
    copyPipe(other);
}

VulkanPipe::VulkanPipe(VulkanPipe &&other) noexcept
    : Pipe(other)
    , m_context(other.m_context)
    , m_renderer(other.m_renderer)
    , m_vertexBuffer(VK_NULL_HANDLE)
    , m_vertexBufferMemory(VK_NULL_HANDLE)
    , m_verticesOffset(0)
    , m_indicesOffset(0)
    , m_vertices()
    , m_indices()
{
    movePipe(other);
}

VulkanPipe &VulkanPipe::operator=(const VulkanPipe &other)
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
        copyPipe(other);
    }
    return *this;
}

VulkanPipe &VulkanPipe::operator=(VulkanPipe &&other) noexcept
{
    if (this != &other)
    {
        movePipe(other);
    }
    return *this;
}

void VulkanPipe::fillCommandBuffer(VkCommandBuffer commandBuffer) const
{
    VkBuffer buff[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {m_verticesOffset};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buff, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vertexBuffer, m_indicesOffset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void VulkanPipe::fillVeticesAndIndices()
{
    float full_distance = 0;
    for (int i = 1; i < m_positions.size(); i++)
    {
        full_distance += glm::distance(m_positions[i], m_positions[i - 1]);
    }

    if (!m_closed)
    {
        // Create base and top center vertices
        m_vertices.push_back({m_positions.front(), {0.5f, 0.5f}, glm::normalize(m_positions.front() - m_positions[1])});
        m_vertices.push_back({m_positions.back(),
                              {0.5f, 0.5f},
                              glm::normalize(m_positions.back() - m_positions[m_positions.size() - 2])});
    }

    // Create contours
    std::vector<std::vector<Vertex3D>> contours;
    contours.push_back(getFirstContour(false));
    float curr_distance = 0;
    for (int i = 1; i < m_positions.size(); i++)
    {
        curr_distance += glm::distance(m_positions[i], m_positions[i - 1]);
        if (m_closed)
        {
            contours.push_back(projectContour(m_positions[i], m_positions[i - 1],
                                              (i == m_positions.size() - 1) ? m_positions[0] : m_positions[i + 1],
                                              contours.back(), curr_distance / full_distance));
        }
        else
        {
            contours.push_back(projectContour(m_positions[i], m_positions[i - 1],
                                              (i == m_positions.size() - 1) ? m_positions[i] : m_positions[i + 1],
                                              contours.back(), curr_distance / full_distance));
        }
    }
    if (m_closed)
    {
        contours.push_back(getFirstContour(true));
    }

    for (const auto &contour : contours)
    {
        for (const auto &vertex : contour)
        {
            m_vertices.push_back(vertex);
        }
    }

    if (!m_closed)
    {
        // Indices for first contour
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
        // Indices for last contour
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
    }
    // Indices for middle contours
    unsigned int k1, k2;
    for (int i = 0; i < contours.size() - 1; ++i)
    {
        k1 = ((m_closed) ? 0 : 2) + i * (m_sectors + 1); // beginning of current contour
        k2 = k1 + m_sectors + 1;                         // beginning of next contour

        for (int j = 0; j < m_sectors; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector
            m_indices.push_back(k1);
            m_indices.push_back(k1 + 1);
            m_indices.push_back(k2);

            m_indices.push_back(k2);
            m_indices.push_back(k1 + 1);
            m_indices.push_back(k2 + 1);
        }
    }
}

std::vector<Vertex3D> VulkanPipe::getFirstContour(bool asLast)
{
    SPARK_CORE_ASSERT(m_positions.size() > 1, "Pipe must have at least two points");
    std::vector<glm::vec3> countour = buildCircle();
    std::vector<Vertex3D> contourVertices;
    glm::mat4 transform = glm::lookAt({0, 0, 0}, m_positions[1] - m_positions[0], {0, 0, 1});
    transform = glm::translate(glm::mat4(1), m_positions[0]) * transform;
    int index = 0;
    for (glm::vec3 &point : countour)
    {
        glm::vec3 position = transform * glm::vec4(point, 1.0f);
        float s = (float)index / m_sectors;
        float t = (asLast) ? 1.0f : 0.0f;
        glm::vec3 normal = glm::normalize(position - m_positions[0]);
        contourVertices.push_back({position, {s, t}, normal});
        index++;
    }
    return contourVertices;
}

std::vector<Vertex3D> VulkanPipe::projectContour(glm::vec3 position, glm::vec3 from, glm::vec3 to,
                                                 std::vector<Vertex3D> previousContour, float distance)
{
    glm::vec3 normal = glm::normalize((position - from) + (to - position));
    std::vector<Vertex3D> countour;
    std::vector<Vertex3D> contourVertices;
    int index = 0;
    for (const Vertex3D &point : previousContour)
    {
        glm::vec3 dir = (position - from);
        float planeD = -(normal.x * position.x + normal.y * position.y + normal.z * position.z);
        float t = -(normal.x * point.pos.x + normal.y * point.pos.y + normal.z * point.pos.z + planeD) /
                  (normal.x * dir.x + normal.y * dir.y + normal.z * dir.z);
        glm::vec3 pointPosition = point.pos + t * dir;
        float tex_s = (float)index / m_sectors;
        float tex_t = distance;
        glm::vec3 pointNormal = glm::normalize(pointPosition - position);
        contourVertices.push_back({pointPosition, {tex_s, tex_t}, pointNormal});
        index++;
    }
    return contourVertices;
}

std::vector<glm::vec3> VulkanPipe::buildCircle()
{
    std::vector<glm::vec3> points;
    float sectorStep = 2 * glm::pi<float>() / m_sectors;
    for (int i = 0; i <= m_sectors; ++i)
    {
        float sectorAngle = i * sectorStep; // starting from 0 to 2pi
        float x = m_radius * glm::cos(sectorAngle);
        float y = m_radius * glm::sin(sectorAngle);
        points.push_back({x, y, 0});
    }
    return points;
}

void VulkanPipe::copyPipe(const VulkanPipe &other)
{
    createVertex3DBuffer(m_context, m_vertexBuffer, m_vertexBufferMemory, m_verticesOffset, m_indicesOffset, m_vertices,
                         m_indices);
}

void VulkanPipe::movePipe(VulkanPipe &other) noexcept
{
    m_vertexBuffer = other.m_vertexBuffer;
    other.m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexBufferMemory = other.m_vertexBufferMemory;
    other.m_vertexBufferMemory = VK_NULL_HANDLE;
    m_verticesOffset = other.m_verticesOffset;
    m_indicesOffset = other.m_indicesOffset;
}
} // namespace Spark::Render
