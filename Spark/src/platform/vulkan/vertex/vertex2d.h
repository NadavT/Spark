#pragma once
#include "platform/vulkan/vulkan_context.h"
#include <array>
#include <vector>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace Spark
{
struct Vertex2D
{
    alignas(8) glm::vec2 pos;
    alignas(16) glm::vec3 color;
    alignas(8) glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2D, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex2D, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex2D &other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

void createVertex2DBuffer(VulkanContext &context, VkBuffer &buffer, VkDeviceMemory &bufferMemory,
                          VkDeviceSize &verticesOffset, VkDeviceSize &indicesOffset,
                          const std::vector<Vertex2D> &bufferVertices, const std::vector<uint32_t> &bufferIndices);
} // namespace Spark
