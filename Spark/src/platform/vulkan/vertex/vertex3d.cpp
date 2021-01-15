#include "vertex3d.h"

namespace Spark::Render
{
VkVertexInputBindingDescription vertex3DGetBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> vertex3DGetAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex3D, texCoord);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex3D, normal);

    return attributeDescriptions;
}

void createVertex3DBuffer(VulkanContext &context, VkBuffer &buffer, VkDeviceMemory &bufferMemory,
                          VkDeviceSize &verticesOffset, VkDeviceSize &indicesOffset,
                          const std::vector<Vertex3D> &bufferVertices, const std::vector<uint32_t> &bufferIndices)
{
    VkDeviceSize verticesSize = sizeof(bufferVertices[0]) * bufferVertices.size();
    VkDeviceSize indicesSize = sizeof(bufferIndices[0]) * bufferIndices.size();
    VkDeviceSize bufferSize = verticesSize + indicesSize;
    verticesOffset = 0;
    indicesOffset = verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    context.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory);

    void *data;
    vkMapMemory(context.m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, bufferVertices.data(), (size_t)verticesSize);
    memcpy((void *)((char *)data + verticesSize), bufferIndices.data(), (size_t)indicesSize);
    vkUnmapMemory(context.m_device, stagingBufferMemory);

    context.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    context.copyBuffer(stagingBuffer, buffer, bufferSize);

    vkDestroyBuffer(context.m_device, stagingBuffer, nullptr);
    vkFreeMemory(context.m_device, stagingBufferMemory, nullptr);
}
} // namespace Spark::Render
