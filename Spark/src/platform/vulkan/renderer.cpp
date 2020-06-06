#include "renderer.h"

#include "spark/core/log.h"
#include "platform/vulkan/framebuffer/framebuffer2d.h"
#include "platform/vulkan/pipeline/pipeline_triangle.h"

namespace Spark
{
	static VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VulkanRenderer::VulkanRenderer(const Window& window)
		: m_context(window)
		, m_width(window.GetWidth())
		, m_height(window.GetHeight())
		, m_semaphores()
		, m_inFlightFences()
		, m_currentFrame(0)
		, m_currentImageIndex(0)
		, m_framebufferResized(false)
		, m_currentCommandsDrawCount(0)
		, m_commandBuffers()
		, m_multisampleImage(VK_NULL_HANDLE)
		, m_multisampleImageMemory(VK_NULL_HANDLE)
		, m_multisampleImageView(VK_NULL_HANDLE)
		, m_recreationNeeded(false)
	{
		for (int i = 0; i < m_inFlightFences.size(); i++)
		{
			m_inFlightFences[i] = m_context.createFence();
		}

		m_imagesInFlight.resize(m_context.m_swapChainImages.size(), VK_NULL_HANDLE);

		if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
		{
			createMultisamplesResources();
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		for (int i = 0; i < m_semaphores.size(); i++)
		{
			for (int j = 0; j < m_semaphores[i].size(); j++)
			{
				m_context.destroySemaphore(m_semaphores[i][j]);
			}
		}

		for (int i = 0; i < m_inFlightFences.size(); i++)
		{
			m_context.destroyFence(m_inFlightFences[i]);
		}

		cleanupMultisamplesResources();
	}

	bool VulkanRenderer::begin()
	{
		m_commandBuffers.clear();

		VkResult result = VK_SUCCESS;
		waitForFence(&m_inFlightFences[m_currentFrame]);

		if (m_semaphores[m_currentFrame].size() == 0)
		{
			m_semaphores[m_currentFrame].push_back(m_context.createSemaphore());
		}
		result = vkAcquireNextImageKHR(m_context.m_device, m_context.m_swapChain,
			UINT64_MAX, m_semaphores[m_currentFrame][0], VK_NULL_HANDLE, &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		if (m_imagesInFlight[m_currentImageIndex] != VK_NULL_HANDLE) {
			waitForFence(&m_imagesInFlight[m_currentImageIndex]);
		}

		m_imagesInFlight[m_currentImageIndex] = m_inFlightFences[m_currentFrame];

		return true;
	}

	void VulkanRenderer::end()
	{
		VkResult result = VK_SUCCESS;

		if (m_commandBuffers.size() > 0)
		{
			std::vector<VkSubmitInfo> infos;

			for (int currentBuffer = 0; currentBuffer < m_commandBuffers.size(); currentBuffer++)
			{
				VkSubmitInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				info.waitSemaphoreCount = 1;
				info.pWaitSemaphores = &m_semaphores[m_currentFrame][currentBuffer];
				info.pWaitDstStageMask = &wait_stage;
				info.commandBufferCount = 1;
				info.pCommandBuffers = &m_commandBuffers[currentBuffer];
				info.signalSemaphoreCount = 1;
				info.pSignalSemaphores = &m_semaphores[m_currentFrame][currentBuffer + 1];

				infos.push_back(info);
			}

			resetFence(&m_inFlightFences[m_currentFrame]);
			queueSubmits(infos.size(), infos.data(), m_inFlightFences[m_currentFrame]);

			VkSwapchainKHR swapChains[] = { m_context.m_swapChain };

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &m_semaphores[m_currentFrame][m_currentCommandsDrawCount];
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &m_currentImageIndex;

			result = vkQueuePresentKHR(m_context.m_presentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
			{
				m_framebufferResized = false;
				recreateSwapchain();
			}
			else if (result != VK_SUCCESS) {
				throw std::runtime_error("failed to present swap chain image!");
			}
			else
			{
				m_recreationNeeded = false;
			}
		}

		m_currentCommandsDrawCount = 0;
		m_currentFrame = (m_currentFrame + 1) % m_inFlightFences.size();
	}

	void VulkanRenderer::render(VkCommandBuffer commandBuffer)
	{
		m_currentCommandsDrawCount++;
		if (m_currentCommandsDrawCount >= m_semaphores[m_currentFrame].size())
		{
			m_semaphores[m_currentFrame].push_back(m_context.createSemaphore());
		}

		m_commandBuffers.push_back(commandBuffer);
	}

	uint32_t VulkanRenderer::getCurrentImageIndex() const
	{
		return m_currentImageIndex;
	}

	uint32_t VulkanRenderer::getImagesAmount() const
	{
		return static_cast<uint32_t>(m_context.m_swapChainImages.size());
	}

	VkImageView VulkanRenderer::getMultisampleImageView() const
	{
		return m_multisampleImageView;
	}

	bool VulkanRenderer::isRecreationNeeded() const
	{
		return m_recreationNeeded;
	}

	void VulkanRenderer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(SPARK_BIND_EVENT_FN(VulkanRenderer::onWindowResize));
	}

	VulkanFramebuffer* VulkanRenderer::createFramebuffer(VulkanFramebufferType type, bool first_layer, bool last_layer)
	{
		if (type == VulkanFramebufferType::Type2D)
		{
			m_framebuffers.push_back(std::make_unique<VulkanFramebuffer2D>(m_context, first_layer, last_layer, m_multisampleImageView));
			return m_framebuffers.back().get();
		}
		else
		{
			SPARK_CORE_ERROR("Not supporting VulkanFramebufferType %d", static_cast<int>(type));
			throw std::runtime_error("Not supporting framebuffer given type!");
		}
	}

	void VulkanRenderer::destroyFramebuffer(VulkanFramebuffer* framebuffer)
	{
		auto found_it = std::find_if(m_framebuffers.begin(), m_framebuffers.end(), [&](std::unique_ptr<VulkanFramebuffer>& p) { return p.get() == framebuffer; });
		if (found_it != m_framebuffers.end())
		{
			m_framebuffers.erase(found_it);
		}
		else
		{
			SPARK_CORE_WARN("Tring to destroy a non existing framebuffer!");
		}
	}

	VulkanPipeline* VulkanRenderer::createPipeline(VulkanPipelineType type, VulkanFramebuffer& framebuffer)
	{
		switch (type)
		{
		//case VulkanPipelineType::Type2D:
		//	m_pipelines.push_back(std::make_unique<VulkanPipeline2D>(m_context, framebuffer));
		//	return m_pipelines.back().get();
		case VulkanPipelineType::TypeTriangle:
			m_pipelines.push_back(std::make_unique<VulkanPipelineTriangle>(m_context, framebuffer));
			return m_pipelines.back().get();
		default:
			SPARK_CORE_ERROR("Not supporting VulkanPipelineType %d", static_cast<int>(type));
			throw std::runtime_error("Not supporting pipeline given type!");
		}
	}

	void VulkanRenderer::destroyPipeline(VulkanPipeline* pipeline)
	{
		auto found_it = std::find_if(m_pipelines.begin(), m_pipelines.end(), [&](std::unique_ptr<VulkanPipeline>& p) { return p.get() == pipeline; });
		if (found_it != m_pipelines.end())
		{
			m_pipelines.erase(found_it);
		}
		else
		{
			SPARK_CORE_WARN("Tring to destroy a non existing framebuffer!");
		}
	}

	bool VulkanRenderer::onWindowResize(WindowResizeEvent& e)
	{
		m_width = e.GetWidth();
		m_height = e.GetHeight();
		m_framebufferResized = true;
		return true;
	}

	void VulkanRenderer::recreateSwapchain()
	{
		if (m_width == 0 || m_height == 0) {
			return;
		}

		waitForIdle();

		for (auto& pipeline : m_pipelines)
		{
			pipeline->cleanup();
		}

		for (auto& framebuffer : m_framebuffers)
		{
			framebuffer->cleanup();
		}

		vkResetCommandPool(m_context.m_device, m_context.m_commandPool, 0);

		cleanupMultisamplesResources();
		m_context.cleanupSwapchain();
		m_context.recreateSwapchain(m_width, m_height);
		if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
		{
			createMultisamplesResources();
		}

		for (auto& framebuffer : m_framebuffers)
		{
			framebuffer->recreate(m_multisampleImageView);
		}

		for (auto& pipeline : m_pipelines)
		{
			pipeline->recreate();
		}

		m_recreationNeeded = true;
	}

	void VulkanRenderer::waitForIdle()
	{
		VkResult result = vkDeviceWaitIdle(m_context.m_device);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkDeviceWaitIdle failed!");
	}

	void VulkanRenderer::beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo* info)
	{
		VkResult result = vkBeginCommandBuffer(commandBuffer, info);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkBeginCommandBuffer failed!");
	}

	void VulkanRenderer::endCommandBuffer(VkCommandBuffer commandBuffer)
	{
		VkResult result = vkEndCommandBuffer(commandBuffer);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkEndCommandBuffer failed!");
	}

	void VulkanRenderer::queueSubmits(uint32_t amount, VkSubmitInfo* info, VkFence fence)
	{
		VkResult result = vkQueueSubmit(m_context.m_graphicsQueue, amount, info, fence);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkQueueSubmit failed!");
	}

	bool VulkanRenderer::accuireNextImage(VkSemaphore semaphore, VkFence fence, uint32_t* imageIndex)
	{
		VkResult result = vkAcquireNextImageKHR(m_context.m_device, m_context.m_swapChain, UINT64_MAX, semaphore, fence, imageIndex);
		if (result == VK_SUCCESS) {
			return true;
		}
		else if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return false;
		}
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkAcquireNextImageKHR failed!");
		return false;
	}

	void VulkanRenderer::waitForFence(const VkFence* fence)
	{
		VkResult result = vkWaitForFences(m_context.m_device, 1, fence, VK_TRUE, UINT64_MAX);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkWaitForFences failed!");
	}

	void VulkanRenderer::resetFence(const VkFence* fence)
	{
		VkResult result = vkResetFences(m_context.m_device, 1, fence);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkResetFences failed!");
	}

	void VulkanRenderer::resetCommandBuffer(VkCommandBuffer commandBuffer)
	{
		VkResult result = vkResetCommandBuffer(commandBuffer, 0);
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkResetCommandBuffer failed!");
	}

	void VulkanRenderer::beginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, uint32_t clearValueCount, const VkClearValue* clearValues)
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = renderPass;
		info.framebuffer = framebuffer;
		info.renderArea.offset = { 0, 0 };
		info.renderArea.extent.width = m_context.m_swapChainExtent.width;
		info.renderArea.extent.height = m_context.m_swapChainExtent.height;
		info.clearValueCount = clearValueCount;
		info.pClearValues = clearValues;
		vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderer::endRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer);
	}

	void VulkanRenderer::queuePresent(const VkPresentInfoKHR* info)
	{
		VkResult result = vkQueuePresentKHR(m_context.m_graphicsQueue, info);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
			return;
		}
		SPARK_CORE_ASSERT(result == VK_SUCCESS, "vkQueuePresentKHR failed!");
	}

	void VulkanRenderer::createUniformBuffers(VkDeviceSize size, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory) {
		VkDeviceSize bufferSize = size;

		uniformBuffers.resize(m_context.m_swapChainImages.size());
		uniformBuffersMemory.resize(m_context.m_swapChainImages.size());

		for (size_t i = 0; i < m_context.m_swapChainImages.size(); i++) {
			m_context.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniformBuffers[i], uniformBuffersMemory[i]);
		}
	}

	void VulkanRenderer::createMultisamplesResources()
	{
		VkFormat colorFormat = m_context.m_swapChainImageFormat;

		m_context.createImage(m_context.m_swapChainExtent.width, m_context.m_swapChainExtent.height,
			1, m_context.m_msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_multisampleImage, m_multisampleImageMemory);
		m_multisampleImageView = m_context.createImageView(m_multisampleImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void VulkanRenderer::cleanupMultisamplesResources()
	{
		if (m_multisampleImageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_context.m_device, m_multisampleImageView, nullptr);
		}

		if (m_multisampleImage != VK_NULL_HANDLE)
		{
			vkDestroyImage(m_context.m_device, m_multisampleImage, nullptr);
		}

		if (m_multisampleImageMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_context.m_device, m_multisampleImageMemory, nullptr);
		}
	}
}