#include "renderer.h"

#include "spark/core/log.h"
#include "platform/vulkan/framebuffer/framebuffer2D.h"

namespace Spark
{
	VulkanRenderer::VulkanRenderer(const Window& window)
		: m_context(window)
		, m_width(window.GetWidth())
		, m_height(window.GetHeight())
		, m_semaphores()
		, m_inFlightFences()
		, m_currentFrame(0)
		, m_currentImageIndex(0)
		, m_framebufferResized(false)
	{
		m_inFlightFences[0] = m_context.createFence();
		m_inFlightFences[1] = m_context.createFence();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		m_context.destroyFence(m_inFlightFences[0]);
		m_context.destroyFence(m_inFlightFences[1]);
	}

	bool VulkanRenderer::begin()
	{
		VkResult result = VK_SUCCESS;
		waitForFence(&m_inFlightFences[m_currentFrame]);

		result = vkAcquireNextImageKHR(m_context.m_device, m_context.m_swapChain,
			UINT64_MAX, *(m_semaphores[m_currentFrame].end()), VK_NULL_HANDLE, &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		resetFence(&m_inFlightFences[m_currentFrame]);
	}

	void VulkanRenderer::end()
	{
		VkResult result = VK_SUCCESS;

		VkSwapchainKHR swapChains[] = { m_context.m_swapChain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &(*(m_semaphores[m_currentFrame].end()));
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

		m_currentFrame = (m_currentFrame + 1) % m_inFlightFences.size();
	}

	void VulkanRenderer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(SPARK_BIND_EVENT_FN(VulkanRenderer::onWindowResize));
	}

	VulkanFramebuffer* VulkanRenderer::createFramebuffer(VulkanFramebufferType type)
	{
		if (type == VulkanFramebufferType::Type2D)
		{
			m_framebuffers.push_back(std::make_unique<VulkanFramebuffer2D>(m_context));
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

		for (auto& framebuffer : m_framebuffers)
		{
			framebuffer->cleanup();
		}

		vkResetCommandPool(m_context.m_device, m_context.m_commandPool, 0);

		m_context.cleanupSwapchain();
		m_context.recreateSwapchain(m_width, m_height);

		for (auto& framebuffer : m_framebuffers)
		{
			framebuffer->recreate();
		}
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

	void VulkanRenderer::queueSubmit(VkSubmitInfo* info, VkFence fence)
	{
		VkResult result = vkQueueSubmit(m_context.m_graphicsQueue, 1, info, fence);
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
}