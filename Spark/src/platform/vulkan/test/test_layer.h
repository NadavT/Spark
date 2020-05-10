#pragma once

#include "spark/core/layer.h"
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/test/quad.h"

namespace Spark
{
	class VulkanTestLayer : public Layer
	{
	public:
		VulkanTestLayer(VulkanRenderer& renderer);
		virtual ~VulkanTestLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnRender();

	private:
		void createDescriptorSets();
	private:
		VulkanRenderer& m_renderer;
		VulkanFramebuffer* m_framebuffer;
		VulkanPipeline* m_pipeline;

		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t mipLevelsTexture;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;

		VkDescriptorSet m_MVPDescriptorSet;
		VkDescriptorSet m_textureDescriptorSet;

		std::unique_ptr<Quad> m_quad;
	};
}
