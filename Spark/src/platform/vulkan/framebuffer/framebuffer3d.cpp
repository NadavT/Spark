#include "framebuffer3d.h"

namespace Spark
{
VulkanFramebuffer3D::VulkanFramebuffer3D(VulkanContext &context, VkImageView renderTarget, bool clear, bool resolve)
    : VulkanFramebuffer(context, renderTarget, clear, resolve)
{
    createDepthImage();
    createRenderPass();
    createSwapchainFramebuffers();
}

VulkanFramebuffer3D::~VulkanFramebuffer3D()
{
    vkDestroyImageView(m_context.m_device, m_depthImageView, nullptr);
    vkDestroyImage(m_context.m_device, m_depthImage, nullptr);
    vkFreeMemory(m_context.m_device, m_depthImageMemory, nullptr);
}

void VulkanFramebuffer3D::cleanup()
{
    vkDestroyImageView(m_context.m_device, m_depthImageView, nullptr);
    vkDestroyImage(m_context.m_device, m_depthImage, nullptr);
    vkFreeMemory(m_context.m_device, m_depthImageMemory, nullptr);
    VulkanFramebuffer::cleanup();
}

void VulkanFramebuffer3D::recreate(VkImageView renderTarget)
{
    VulkanFramebuffer::recreate(renderTarget);
    createDepthImage();
    createRenderPass();
    createSwapchainFramebuffers();
}

VkImage VulkanFramebuffer3D::getDepthImage() const
{
    return m_depthImage;
}

VkImageView VulkanFramebuffer3D::getDepthImageView() const
{
    return m_depthImageView;
}

VkRect2D VulkanFramebuffer3D::getDepthImageDimentions() const
{
    return {0, 0, m_context.m_swapChainExtent.width, m_context.m_swapChainExtent.height};
}

void VulkanFramebuffer3D::createDepthImage()
{
    VkFormat depthFormat = m_context.findDepthFormat();

    m_context.createImage(m_context.m_swapChainExtent.width, m_context.m_swapChainExtent.height, 1,
                          m_context.m_msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          m_depthImage, m_depthImageMemory);
    m_depthImageView = m_context.createImageView(m_depthImage, depthFormat,
                                                 VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 1);
}

void VulkanFramebuffer3D::createRenderPass()
{
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkAttachmentReference> attachmentsRefs;
    attachments.reserve(2);
    attachmentsRefs.reserve(2);
    VkSubpassDescription subpass = getBasicSubpass(attachments, attachmentsRefs);

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = m_context.findDepthFormat();
    depthAttachment.samples = m_context.m_msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments.push_back(depthAttachment);

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = static_cast<uint32_t>(attachments.size() - 1);
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentsRefs.push_back(depthAttachmentRef);

    subpass.pDepthStencilAttachment = &(attachmentsRefs.back());

    VkSubpassDependency dependency = getBasicSubpassDependency();

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_context.m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanFramebuffer3D::createSwapchainFramebuffers()
{
    m_swapChainFramebuffers.resize(m_context.m_swapChainImageViews.size());

    for (size_t i = 0; i < m_context.m_swapChainImageViews.size(); i++)
    {
        std::vector<VkImageView> attachments = getBasicFramebufferAttachments(m_context.m_swapChainImageViews[i]);
        attachments.push_back(m_depthImageView);

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_context.m_swapChainExtent.width;
        framebufferInfo.height = m_context.m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_context.m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
} // namespace Spark
