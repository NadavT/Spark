#include "layer2d.h"

#include "platform/vulkan/pipeline/pipeline2d.h"

namespace Spark
{
	VulkanLayer2D::VulkanLayer2D(VulkanRenderer& renderer)
		: Layer("2d layer")
		, m_renderer(renderer)
		, m_framebuffer(nullptr)
		, m_pipeline(nullptr)
		, m_quad(nullptr)
		, m_uniformTransformations()
		, m_uniformTransformationsMemory()
		, m_transfomationDescriptorSets()
		, m_texture(nullptr)
		, m_sampler(nullptr)
		, m_textureDescriptorSets()
		, m_commandBuffers()
	{
		m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type2D);
		m_pipeline = reinterpret_cast<VulkanPipeline2D*>(renderer.createPipeline(VulkanPipelineType::Type2D, *m_framebuffer));
		m_commandBuffers.resize(renderer.getImagesAmount());
		m_quad = std::make_unique<Quad>(m_renderer.m_context, glm::vec2(-0.5, -0.5));
		m_renderer.createUniformBuffers(sizeof(Transformation2D), m_uniformTransformations, m_uniformTransformationsMemory);
		m_texture = std::make_unique<VulkanTexture>(renderer.m_context, "texture", "C:\\Users\\NadavT\\Pictures\\poke.png");
		m_sampler = std::make_unique<VulkanTextureSampler>(renderer.m_context, "sampler");
		m_pipeline->createDescriptorSets(m_transfomationDescriptorSets, m_uniformTransformations, m_textureDescriptorSets, m_texture->getImageView(), m_sampler->getSampler());
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
		}
	}

	VulkanLayer2D::~VulkanLayer2D()
	{
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
		}
		for (size_t i = 0; i < m_uniformTransformations.size(); i++)
		{
			vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformTransformations[i], nullptr);
			vkFreeMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i], nullptr);
		}
		m_renderer.destroyPipeline(m_pipeline);
		m_pipeline = nullptr;
		m_renderer.destroyFramebuffer(m_framebuffer);
		m_framebuffer = nullptr;
	}

	void VulkanLayer2D::OnAttach()
	{
		createCommandBuffers();
	}

	void VulkanLayer2D::OnDetach()
	{
		for (VkCommandBuffer commandBuffer : m_commandBuffers) {
			m_renderer.resetCommandBuffer(commandBuffer);
		}
	}

	void VulkanLayer2D::OnUpdate(Time& diffTime)
	{
	}

	void VulkanLayer2D::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SPARK_BIND_EVENT_FN(VulkanLayer2D::handleKeyPressed));
	}

	void VulkanLayer2D::OnRender()
	{
		if (m_renderer.isRecreationNeeded())
		{
			for (VkCommandBuffer commandBuffer : m_commandBuffers) {
				m_renderer.resetCommandBuffer(commandBuffer);
			}
			createCommandBuffers();
		}

		void* data;
		struct Transformation2D transformation = {};
		transformation.transformMatrix = glm::mat4(m_quad->getTransformation());
		vkMapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[m_renderer.getCurrentImageIndex()], 0, sizeof(transformation), 0, &data);
		memcpy(data, &transformation, sizeof(transformation));
		vkUnmapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[m_renderer.getCurrentImageIndex()]);

		VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
		m_renderer.render(commandBuffer);
	}

	void VulkanLayer2D::createCommandBuffers()
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

			m_pipeline->bind(commandBuffer, m_transfomationDescriptorSets[i], m_textureDescriptorSets[i]);

			m_quad->fillCommandBuffer(commandBuffer);

			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
			i++;
		}

	}

	bool VulkanLayer2D::handleKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case KeyCode::Left:
			m_quad->move({-0.05, 0});
			return true;
		case KeyCode::Right:
			m_quad->move({0.05, 0});
			return true;
		case KeyCode::Down:
			m_quad->move({0, 0.05});
			return true;
		case KeyCode::Up:
			m_quad->move({0, -0.05});
			return true;
		default:
			return false;
		}
	}
}
