#ifndef SPARK_VULKAN_VERTEX3D_H
#define SPARK_VULKAN_VERTEX3D_H
#include "platform/vulkan/vulkan_context.h"
#include "spark/vertex/vertex3d.h"
#include <array>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Spark::Render
{
VkVertexInputBindingDescription vertex3DGetBindingDescription();

std::array<VkVertexInputAttributeDescription, 3> vertex3DGetAttributeDescriptions();

void createVertex3DBuffer(VulkanContext &context, VkBuffer &buffer, VkDeviceMemory &bufferMemory,
                          VkDeviceSize &verticesOffset, VkDeviceSize &indicesOffset,
                          const std::vector<Vertex3D> &bufferVertices, const std::vector<uint32_t> &bufferIndices);
} // namespace Spark::Render

#endif /* SPARK_VULKAN_VERTEX3D_H */
