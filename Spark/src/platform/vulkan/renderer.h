#pragma once

#include "spark/core/window.h"
#include "spark/event/application_event.h"
#include "spark/renderer/renderer.h"

#include "platform/vulkan/framebuffer/framebuffer.h"
#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vulkan_context.h"

#include <array>
#include <vector>

namespace Spark
{
class VulkanRenderer : public Renderer
{
  public:
    VulkanRenderer(const Window &window);
    virtual ~VulkanRenderer();

    VulkanContext &getContext();

    virtual bool begin();
    virtual void end();

    void render(VkCommandBuffer commandBuffer);
    uint32_t getCurrentImageIndex() const;
    uint32_t getImagesAmount() const;
    VkImageView getMultisampleImageView() const;
    bool isRecreationNeeded() const;
    void signalRecreationNeeded();

    virtual void OnEvent(Event &e);
    VulkanFramebuffer *createFramebuffer(VulkanFramebufferType type, bool clear = false, bool resolve = false);
    void destroyFramebuffer(VulkanFramebuffer *framebuffer);
    VulkanPipeline *createPipeline(VulkanPipelineType type, VulkanFramebuffer &framebuffer);
    void destroyPipeline(VulkanPipeline *pipeline);
    void recreateSwapchain();

    virtual void waitForIdle();

    void beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo *info);
    void endCommandBuffer(VkCommandBuffer commandBuffer);
    void queueSubmits(uint32_t amount, VkSubmitInfo *info, VkFence fence);
    bool accuireNextImage(VkSemaphore semaphore, VkFence fence, uint32_t *imageIndex);
    void waitForFence(const VkFence *fence);
    void resetFence(const VkFence *fence);
    void resetCommandBuffer(VkCommandBuffer commandBuffer);
    void beginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer,
                         uint32_t clearValueCount = 0, const VkClearValue *clearValues = VK_NULL_HANDLE);
    void endRenderPass(VkCommandBuffer commandBuffer);
    void queuePresent(const VkPresentInfoKHR *info);
    void createUniformBuffers(VkDeviceSize size, std::vector<VkBuffer> &uniformBuffers,
                              std::vector<VkDeviceMemory> &uniformBuffersMemory);
    void createUniformBuffers(VkDeviceSize size, std::vector<std::vector<VkBuffer>> &uniformBuffers,
                              std::vector<std::vector<VkDeviceMemory>> &uniformBuffersMemory, unsigned int amount);
    void addUniformBuffers(VkDeviceSize size, std::vector<std::vector<VkBuffer>> &uniformBuffers,
                           std::vector<std::vector<VkDeviceMemory>> &uniformBuffersMemory);

  private:
    bool onWindowResize(WindowResizeEvent &e);
    void createMultisamplesResources();
    void cleanupMultisamplesResources();
    void createClearCommandBuffers();
    void createResolveCommandBuffers();
    void fillClearCommandBuffers();
    void fillResolveCommandBuffers();

  public:
    VulkanContext m_context;
    std::vector<std::unique_ptr<VulkanFramebuffer>> m_framebuffers;
    std::vector<std::unique_ptr<VulkanPipeline>> m_pipelines;
    int m_width;
    int m_height;
    std::array<std::vector<VkSemaphore>, 2> m_semaphores;
    std::array<VkFence, 2> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    uint32_t m_currentFrame;
    uint32_t m_currentImageIndex;
    bool m_framebufferResized;
    uint32_t m_currentCommandsDrawCount;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkImage m_multisampleImage;
    VkDeviceMemory m_multisampleImageMemory;
    VkImageView m_multisampleImageView;
    VulkanFramebuffer *m_clearFramebuffer;
    VulkanFramebuffer *m_resolveFramebuffer;
    bool m_recreationNeeded;
    std::vector<VkCommandBuffer> m_clearCommandBuffers;
    std::vector<VkCommandBuffer> m_resolveCommandBuffers;
};
} // namespace Spark
