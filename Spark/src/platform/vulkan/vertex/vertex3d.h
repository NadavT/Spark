#pragma once
#include "platform/vulkan/vulkan_context.h"
#include <array>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Spark
{
	struct Vertex3D {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
		glm::vec3 normal;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3D);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3D, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex3D, normal);

			return attributeDescriptions;
		}

		bool operator==(const Vertex3D& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	void createVertex3DBuffer(VulkanContext& context, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDeviceSize& verticesOffset, VkDeviceSize& indicesOffset,
		const std::vector<Vertex3D>& bufferVertices, const std::vector<uint32_t>& bufferIndices);
}

template<> struct std::hash<Spark::Vertex3D> {
	size_t operator()(Spark::Vertex3D const& vertex) const {
		return ((std::hash<glm::vec3>()(vertex.pos) ^
			(std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			(std::hash<glm::vec2>()(vertex.texCoord) << 1);
	}
};
