#include "vertex3d.h"

namespace Spark::Render
{
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
