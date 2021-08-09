#include "framebuffer.h"

namespace Spark::Render
{
VulkanFramebuffer::VulkanFramebuffer(VulkanContext &context, VkImageView renderTarget, bool clear, bool resolve)
    : m_clear(clear)
    , m_resolve(resolve)
    , m_context(context)
    , m_renderPass(VK_NULL_HANDLE)
    , m_swapChainFramebuffers()
    , m_renderTarget(renderTarget)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    VulkanFramebuffer::cleanup();
}

void VulkanFramebuffer::cleanup()
{
    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_context.m_device, m_swapChainFramebuffers[i], nullptr);
    }

    vkDestroyRenderPass(m_context.m_device, m_renderPass, nullptr);
}

void VulkanFramebuffer::recreate(VkImageView renderTarget)
{
    m_renderTarget = renderTarget;
    m_renderPass = VK_NULL_HANDLE;
    m_swapChainFramebuffers.resize(0);
}

VkRenderPass VulkanFramebuffer::getRenderPass() const
{
    return m_renderPass;
}

const std::vector<VkFramebuffer> VulkanFramebuffer::getswapChainFramebuffers() const
{
    return m_swapChainFramebuffers;
}

VkSubpassDescription VulkanFramebuffer::getBasicSubpass(std::vector<VkAttachmentDescription> &attachments,
                                                        std::vector<VkAttachmentReference> &attachmentsRefs)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_context.m_swapChainImageFormat;
    colorAttachment.samples = m_context.m_msaaSamples;
    colorAttachment.loadOp = (m_clear) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    if (m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        colorAttachment.initialLayout =
            (m_clear) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else
    {
        colorAttachment.initialLayout = (m_clear) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    attachments.push_back(colorAttachment);

    if (m_resolve && m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentDescription colorAttachmentResolve = {};
        colorAttachmentResolve.format = m_context.m_swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachments.push_back(colorAttachmentResolve);
    }

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentsRefs.push_back(colorAttachmentRef);

    if (m_resolve && m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentReference colorAttachmentResolveRef = {};
        colorAttachmentResolveRef.attachment = 1;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentsRefs.push_back(colorAttachmentResolveRef);
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachmentsRefs[0];
    if (m_resolve && m_context.m_msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        subpass.pResolveAttachments = &attachmentsRefs[1];
    }

    return subpass;
}

VkSubpassDependency VulkanFramebuffer::getBasicSubpassDependency()
{
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    return dependency;
}

std::vector<VkImageView> VulkanFramebuffer::getBasicFramebufferAttachments(VkImageView currentImageView)
{
    std::vector<VkImageView> attachments;
    if (m_renderTarget != VK_NULL_HANDLE)
    {
        attachments.push_back(m_renderTarget);
        if (m_resolve)
        {
            attachments.push_back(currentImageView);
        }
    }
    else
    {
        attachments.push_back(currentImageView);
    }

    return attachments;
}
} // namespace Spark::Render
