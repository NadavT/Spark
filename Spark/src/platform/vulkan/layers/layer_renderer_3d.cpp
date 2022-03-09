#include "layer_renderer_3d.h"
#include "platform/vulkan/drawables/textured_box.h"

namespace Spark::Render
{
VulkanLayerRenderer3D::VulkanLayerRenderer3D(VulkanRenderer &renderer, Camera &camera)
    : m_renderer(renderer)
    , m_framebuffer(nullptr)
    , m_pipeline(nullptr)
    , m_uniformTransformations()
    , m_uniformTransformationsMemory()
    , m_transformationDescriptorSets()
    , m_textureDescriptorSets()
    , m_commandBuffers()
    , m_toBeRemoved()
    , m_isAttached(false)
    , m_isRecreationNeeded(false)
    , m_camera(&camera)
{
    m_framebuffer = renderer.createFramebuffer(VulkanFramebufferType::Type3D);
    m_pipeline =
        reinterpret_cast<VulkanPipeline3D *>(renderer.createPipeline(VulkanPipelineType::Type3D, *m_framebuffer));
    m_commandBuffers.resize(renderer.getImagesAmount());
    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
    }
}

VulkanLayerRenderer3D::~VulkanLayerRenderer3D()
{
    if (m_isAttached)
    {
        VulkanLayerRenderer3D::OnDetach();
    }

    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
    }

    m_renderer.destroyPipeline(m_pipeline);
    m_pipeline = nullptr;
    m_renderer.destroyFramebuffer(m_framebuffer);
    m_framebuffer = nullptr;
}

void VulkanLayerRenderer3D::OnAttach()
{
    std::vector<VkImageView> textures;
    std::vector<VkSampler> samplers;
    m_renderer.createUniformBuffers(sizeof(Transformation3D), m_uniformTransformations, m_uniformTransformationsMemory,
                                    (unsigned int)m_drawables.size());
    m_pipeline->createTransformationDescriptorSets((unsigned int)m_drawables.size(), m_transformationDescriptorSets,
                                                   m_uniformTransformations);
    for (auto &drawable : m_drawables)
    {
        VulkanTexturedBox *quad = dynamic_cast<VulkanTexturedBox *>(drawable.get());
        if (m_textureDescriptorOffset.find(quad->getTexture().getName()) == m_textureDescriptorOffset.end())
        {
            m_textureDescriptorOffset[quad->getTexture().getName()] = (unsigned int)textures.size();
            textures.push_back(quad->getTexture().getImage().getImageView());
            samplers.push_back(quad->getTexture().getSampler().getSampler());
        }
    }

    m_pipeline->createTextureDescriptorSets((unsigned int)textures.size(), m_textureDescriptorSets, textures, samplers);
    createCommandBuffers();
    m_isAttached = true;
}

void VulkanLayerRenderer3D::OnDetach()
{
    m_renderer.waitForIdle();
    m_isAttached = false;
    for (VkCommandBuffer commandBuffer : m_commandBuffers)
    {
        m_renderer.resetCommandBuffer(commandBuffer);
    }

    for (size_t i = 0; i < m_textureDescriptorSets.size(); i++)
    {
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_textureDescriptorSets[i].size(), m_textureDescriptorSets[i].data());
    }

    m_textureDescriptorOffset.clear();

    for (size_t i = 0; i < m_uniformTransformations.size(); i++)
    {
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_transformationDescriptorSets[i].size(),
                             m_transformationDescriptorSets[i].data());
        for (size_t j = 0; j < m_uniformTransformations[i].size(); j++)
        {
            vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformTransformations[i][j], nullptr);
            vkFreeMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i][j], nullptr);
        }
    }
}

void VulkanLayerRenderer3D::OnRender()
{
    if (m_isRecreationNeeded || m_renderer.isRecreationNeeded())
    {
        for (VkCommandBuffer commandBuffer : m_commandBuffers)
        {
            m_renderer.resetCommandBuffer(commandBuffer);
        }

        while (!m_toBeRemoved.empty())
        {
            LayerRenderer::removeDrawable(m_toBeRemoved.back());
            m_toBeRemoved.pop_back();
        }

        createCommandBuffers();
        m_isRecreationNeeded = false;
    }

    for (size_t i = 0; i < m_drawables.size(); i++)
    {
        VulkanTexturedBox *cube = dynamic_cast<VulkanTexturedBox *>(m_drawables[i].get());
        void *data;
        struct Transformation3D transformation = {};
        transformation.model = cube->getTransformation();
        transformation.view = m_camera->getViewMatrix();
        transformation.projection = glm::perspective(m_camera->getZoomRadians(),
                                                     m_renderer.m_context.m_swapChainExtent.width /
                                                         (float)m_renderer.m_context.m_swapChainExtent.height,
                                                     0.1f, 100.0f);
        transformation.projection[1][1] *= -1;
        vkMapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i][m_renderer.getCurrentImageIndex()],
                    0, sizeof(transformation), 0, &data);
        memcpy(data, &transformation, sizeof(transformation));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformTransformationsMemory[i][m_renderer.getCurrentImageIndex()]);
    }

    VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
    m_renderer.render(commandBuffer);
}

void VulkanLayerRenderer3D::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    LayerRenderer::addDrawable(drawable);

    VulkanTexturedBox *cube = dynamic_cast<VulkanTexturedBox *>(drawable.get());
    if (m_isAttached)
    {
        if (m_uniformTransformations.size() < m_drawables.size())
        {
            m_renderer.addUniformBuffers(sizeof(Transformation3D), m_uniformTransformations,
                                         m_uniformTransformationsMemory);
            m_pipeline->createSingleTransformationDescriptorSet(m_transformationDescriptorSets,
                                                                m_uniformTransformations.back());
        }
        if (m_textureDescriptorOffset.find(cube->getTexture().getName()) == m_textureDescriptorOffset.end())
        {
            const VulkanTexture &texture = cube->getTexture();
            m_textureDescriptorOffset[cube->getTexture().getName()] =
                static_cast<unsigned int>(m_textureDescriptorSets.size());
            m_pipeline->createSingleTextureDescriptorSet(m_textureDescriptorSets, texture.getImage().getImageView(),
                                                         texture.getSampler().getSampler());
        }

        m_isRecreationNeeded = true;
    }
}

void VulkanLayerRenderer3D::removeDrawable(Drawable *drawable)
{
    m_toBeRemoved.push_back(drawable);
    m_isRecreationNeeded = true;
}

void VulkanLayerRenderer3D::createCommandBuffers()
{
    int i = 0;
    for (VkCommandBuffer commandBuffer : m_commandBuffers)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        std::array<VkClearValue, 2> clearValues{};
        clearValues[1].depthStencil = {1.0f, 0};

        m_renderer.beginRenderPass(commandBuffer, m_framebuffer->getRenderPass(),
                                   m_framebuffer->getswapChainFramebuffers()[i], 2, clearValues.data());

        for (size_t j = 0; j < m_drawables.size(); j++)
        {
            VulkanTexturedBox *cube = dynamic_cast<VulkanTexturedBox *>(m_drawables[j].get());
            m_pipeline->bind(commandBuffer, m_transformationDescriptorSets[j][i],
                             m_textureDescriptorSets[m_textureDescriptorOffset[cube->getTexture().getName()]][i]);
            cube->fillCommandBuffer(commandBuffer);
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
        i++;
    }
}
} // namespace Spark::Render
