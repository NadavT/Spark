#pragma once

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex2d.h"

namespace Spark
{
	struct Transformation2D {
		glm::mat4 transformMatrix;
	};

	class VulkanPipeline2D : public VulkanPipeline
	{
	public:
		VulkanPipeline2D(VulkanContext& context, VulkanFramebuffer& framebuffer);
		virtual ~VulkanPipeline2D();

		virtual void cleanup();
		virtual void recreate();

		virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet textureSet);

		VkDescriptorSetLayout getMVPDescriptorSetLayout();
		VkDescriptorSetLayout getTextureDescriptorSetLayout();

		void createDescriptorSets(std::vector<VkDescriptorSet>& transformationSets, std::vector<VkBuffer> transformationUniforms,
			std::vector<VkDescriptorSet>& textureSets, VkImageView textureImageView, VkSampler textureSampler);

	private:
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
	private:
		VkPipelineLayout m_pipelineLayout;
		VkDescriptorSetLayout m_transformationDescriptorSetLayout;
		VkDescriptorSetLayout m_textureDescriptorSetLayout;
	};
}
