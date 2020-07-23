#include "layer_renderer_2d.h"
#include "platform/vulkan/drawables/quad.h"

namespace Spark
{
	VulkanLayerRenderer2D::VulkanLayerRenderer2D(VulkanRenderer& renderer)
		: m_renderer(renderer)
		, m_framebuffer(nullptr)
		, m_pipeline(nullptr)
		, m_uniformTransformations()
		, m_uniformTransformationsMemory()
		, m_transfomationDescriptorSets()
		, m_textureDescriptorSets()
		, m_commandBuffers()
	{
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D);
		m_pipeline = reinterpret_cast<VulkanPipeline2D*>(renderer.createPipeline(VulkanPipelineType::Type2D, *m_framebuffer));
		m_commandBuffers.resize(renderer.getImagesAmount());
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
	}

	VulkanLayerRenderer2D::~VulkanLayerRenderer2D()
	{
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
		}
		for (size_t i = 0; i < m_uniformTransformations.size(); i++)
		{
			for (size_t j = 0; j < m_uniformTransformations[i].size(); j++)
			{
				vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformTransformations[i][j], nullptr);
				vkFreeMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i][j], nullptr);
			}
		}
		m_renderer.destroyPipeline(m_pipeline);
		m_pipeline = nullptr;
		m_renderer.destroyFramebuffer(m_framebuffer);
		m_framebuffer = nullptr;
	}

	void VulkanLayerRenderer2D::OnAttach()
	{
		std::vector<VkImageView> textures;
		std::vector<VkSampler> samplers;
		m_renderer.createUniformBuffers(sizeof(Transformation2D), m_uniformTransformations, m_uniformTransformationsMemory, m_drawables.size());
		m_pipeline->createTransformationDescriptorSets(m_drawables.size(), m_transfomationDescriptorSets, m_uniformTransformations);
		for (auto* drawable : m_drawables)
		{
			VulkanQuad* quad = reinterpret_cast<VulkanQuad*>(m_drawables[0]);
			textures.push_back(quad->getTexture().getImageView());
			samplers.push_back(quad->getSampler().getSampler());
		}
		
		m_pipeline->createTextureDescriptorSets(m_drawables.size(), m_textureDescriptorSets, textures, samplers);
		createCommandBuffers();
	}

	void VulkanLayerRenderer2D::OnDetach()
	{
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			m_renderer.resetCommandBuffer(commandBuffer);
		}
	}

	void VulkanLayerRenderer2D::OnRender()
	{
		if (m_renderer.isRecreationNeeded())
		{
			for (VkCommandBuffer commandBuffer : m_commandBuffers) {
				m_renderer.resetCommandBuffer(commandBuffer);
			}
			createCommandBuffers();
		}

		int i = 0;

		for (Drawable* drawable : m_drawables)
		{
			VulkanQuad* quad = reinterpret_cast<VulkanQuad*>(drawable);
			void* data;
			struct Transformation2D transformation = {};
			transformation.transformMatrix = glm::mat4(quad->getTransformation());
			vkMapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[m_renderer.getCurrentImageIndex()][i], 0, sizeof(transformation), 0, &data);
			memcpy(data, &transformation, sizeof(transformation));
			vkUnmapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[m_renderer.getCurrentImageIndex()][i]);
			i++;
		}

		VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
		m_renderer.render(commandBuffer);
	}

	void VulkanLayerRenderer2D::createCommandBuffers()
	{
		int i = 0;
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
				m_framebuffer->getswapChainFramebuffers()[i]);

			for (size_t j = 0; j < m_drawables.size(); j++)
			{
				m_pipeline->bind(commandBuffer, m_transfomationDescriptorSets[i][j], m_textureDescriptorSets[i][j]);
				reinterpret_cast<VulkanQuad*>(m_drawables[j])->fillCommandBuffer(commandBuffer);
			}

			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
			i++;
		}
	}
}
