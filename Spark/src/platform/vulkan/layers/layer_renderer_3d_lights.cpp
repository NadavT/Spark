#include "layer_renderer_3d_lights.h"
#include "platform/vulkan/drawables/colored_cube.h"
#include "platform/vulkan/drawables/textured_cube.h"

#include "spark/core/log.h"

#include <algorithm>

namespace Spark
{
VulkanLayerRenderer3DLights::VulkanLayerRenderer3DLights(VulkanRenderer &renderer, Camera &camera)
    : m_renderer(renderer)
    , m_framebuffer(nullptr)
    , m_pipeline(nullptr)
    , m_uniformTransformations()
    , m_uniformTransformationsMemory()
    , m_uniformOutlineTransformations()
    , m_uniformOutlineTransformationsMemory()
    , m_uniformDirectionalLightBuffers()
    , m_uniformDirectionalLightBuffersMemory()
    , m_uniformPointLightBuffers()
    , m_uniformPointLightBuffersMemory()
    , m_uniformSpotLightBuffers()
    , m_uniformSpotLightBuffersMemory()
    , m_transformationDescriptorSets()
    , m_outlineTransformationDescriptorSets()
    , m_lightsDescriptorSets()
    , m_materialDescriptorSets()
    , m_textureDescriptorSets()
    , m_commandBuffers()
    , m_toBeRemoved()
    , m_isAttached(false)
    , m_isRecreationNeeded(false)
    , m_camera(camera)
    , m_dirLightDirection({0.0f, 0.0f, 0.0f})
    , m_dirLightColor({1.0f, 1.0f, 1.0f})
    , m_spotLightColor({1.0f, 1.0f, 1.0f})
    , m_pointLights()
    , m_wireframe(WireframeState::None)
{
    m_framebuffer = reinterpret_cast<VulkanFramebuffer3D *>(renderer.createFramebuffer(VulkanFramebufferType::Type3D));
    m_pipeline = reinterpret_cast<VulkanPipeline3DLights *>(
        renderer.createPipeline(VulkanPipelineType::Type3DLights, *m_framebuffer));
    m_outlinePipeline = reinterpret_cast<VulkanPipeline3DOutline *>(
        renderer.createPipeline(VulkanPipelineType::Type3DOutline, *m_framebuffer));
    m_wireframePipeline = reinterpret_cast<VulkanPipeline3DWireframe *>(
        renderer.createPipeline(VulkanPipelineType::Type3DWireframe, *m_framebuffer));
    m_commandBuffers.resize(renderer.getImagesAmount());
    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
    }
}

VulkanLayerRenderer3DLights::~VulkanLayerRenderer3DLights()
{
    if (m_isAttached)
    {
        OnDetach();
    }

    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
    }

    m_renderer.destroyPipeline(m_outlinePipeline);
    m_outlinePipeline = nullptr;
    m_renderer.destroyPipeline(m_pipeline);
    m_pipeline = nullptr;
    m_renderer.destroyFramebuffer(m_framebuffer);
    m_framebuffer = nullptr;
}

void VulkanLayerRenderer3DLights::OnAttach()
{
    std::vector<VkImageView> textures;
    std::vector<VkSampler> samplers;
    std::vector<VkImageView> specularTextures;
    std::vector<VkSampler> specularSamplers;
    m_renderer.createUniformBuffers(sizeof(Transformation3DLights), m_uniformTransformations,
                                    m_uniformTransformationsMemory, (unsigned int)m_drawables.size());
    m_pipeline->createTransformationDescriptorSets((unsigned int)m_drawables.size(), m_transformationDescriptorSets,
                                                   m_uniformTransformations);
    m_renderer.createUniformBuffers(sizeof(Transformation3DOutline), m_uniformOutlineTransformations,
                                    m_uniformOutlineTransformationsMemory, (unsigned int)m_drawables.size());
    m_outlinePipeline->createTransformationDescriptorSets(
        (unsigned int)m_drawables.size(), m_outlineTransformationDescriptorSets, m_uniformOutlineTransformations);

    m_renderer.createUniformBuffers(sizeof(VulkanShaderDirectionalLight), m_uniformDirectionalLightBuffers,
                                    m_uniformDirectionalLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderPointLight) * MAX_POINT_LIGHTS, m_uniformPointLightBuffers,
                                    m_uniformPointLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderSpotLight), m_uniformSpotLightBuffers,
                                    m_uniformSpotLightBuffersMemory);
    m_pipeline->createLightDescriptorSets(m_lightsDescriptorSets, m_uniformDirectionalLightBuffers,
                                          m_uniformPointLightBuffers, m_uniformSpotLightBuffers);

    for (auto drawable : m_drawables)
    {
        Cube *cube = dynamic_cast<Cube *>(drawable.get());
        if (cube->getType() == CubeType::TexturedCude)
        {
            VulkanTexturedCube *texturedCube = dynamic_cast<VulkanTexturedCube *>(cube);
            if (m_textureDescriptorOffset.find(texturedCube->getTexture().getName()) == m_textureDescriptorOffset.end())
            {
                m_textureDescriptorOffset[texturedCube->getTexture().getName()] = (unsigned int)textures.size();
                textures.push_back(texturedCube->getTexture().getImage().getImageView());
                samplers.push_back(texturedCube->getTexture().getSampler().getSampler());
                specularTextures.push_back(texturedCube->getSpecularTexture().getImage().getImageView());
                specularSamplers.push_back(texturedCube->getSpecularTexture().getSampler().getSampler());
            }
        }
    }
    m_renderer.createUniformBuffers(sizeof(Material), m_uniformMaterialBuffers, m_uniformMaterialBuffersMemory,
                                    (unsigned int)m_drawables.size());
    m_pipeline->createMaterialDescriptorSets((unsigned int)m_drawables.size(), m_materialDescriptorSets,
                                             m_uniformMaterialBuffers);

    m_pipeline->createTextureDescriptorSets((unsigned int)textures.size(), m_textureDescriptorSets, textures, samplers,
                                            specularTextures, specularSamplers);
    createCommandBuffers();
    m_isAttached = true;
}

void VulkanLayerRenderer3DLights::OnDetach()
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

    for (size_t i = 0; i < m_lightsDescriptorSets.size(); i++)
    {
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_lightsDescriptorSets[i].size(), m_lightsDescriptorSets[i].data());
    }

    for (size_t i = 0; i < m_uniformDirectionalLightBuffers.size(); i++)
    {
        vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformSpotLightBuffers[i], nullptr);
        vkFreeMemory(m_renderer.m_context.m_device, m_uniformSpotLightBuffersMemory[i], nullptr);
        vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformPointLightBuffers[i], nullptr);
        vkFreeMemory(m_renderer.m_context.m_device, m_uniformPointLightBuffersMemory[i], nullptr);
        vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformDirectionalLightBuffers[i], nullptr);
        vkFreeMemory(m_renderer.m_context.m_device, m_uniformDirectionalLightBuffersMemory[i], nullptr);
    }

    for (size_t i = 0; i < m_uniformTransformations.size(); i++)
    {
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_transformationDescriptorSets[i].size(),
                             m_transformationDescriptorSets[i].data());
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_materialDescriptorSets[i].size(), m_materialDescriptorSets[i].data());
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_outlineTransformationDescriptorSets[i].size(),
                             m_outlineTransformationDescriptorSets[i].data());
        for (size_t j = 0; j < m_uniformMaterialBuffers[i].size(); j++)
        {
            vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformMaterialBuffers[i][j], nullptr);
            vkFreeMemory(m_renderer.m_context.m_device, m_uniformMaterialBuffersMemory[i][j], nullptr);
        }
        for (size_t j = 0; j < m_uniformTransformations[i].size(); j++)
        {
            vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformTransformations[i][j], nullptr);
            vkFreeMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i][j], nullptr);
        }
        for (size_t j = 0; j < m_uniformOutlineTransformations[i].size(); j++)
        {
            vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformOutlineTransformations[i][j], nullptr);
            vkFreeMemory(m_renderer.m_context.m_device, m_uniformOutlineTransformationsMemory[i][j], nullptr);
        }
    }
}

void VulkanLayerRenderer3DLights::OnRender()
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

    int i = 0;

    for (size_t i = 0; i < m_drawables.size(); i++)
    {
        Cube *cube = dynamic_cast<Cube *>(m_drawables[i].get());
        void *data;
        struct Transformation3DLights transformation = {};
        transformation.model = cube->getTransformation();
        transformation.view = m_camera.getViewMatrix();
        transformation.projection = glm::perspective(m_camera.getZoom(),
                                                     m_renderer.m_context.m_swapChainExtent.width /
                                                         (float)m_renderer.m_context.m_swapChainExtent.height,
                                                     0.1f, 100.0f);
        transformation.projection[1][1] *= -1;
        vkMapMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[i][m_renderer.getCurrentImageIndex()],
                    0, sizeof(transformation), 0, &data);
        memcpy(data, &transformation, sizeof(transformation));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformTransformationsMemory[i][m_renderer.getCurrentImageIndex()]);

        transformation.model =
            glm::scale(transformation.model,
                       {1 + cube->getHighlightWidth(), 1 + cube->getHighlightWidth(), 1 + cube->getHighlightWidth()});
        vkMapMemory(m_renderer.m_context.m_device,
                    m_uniformOutlineTransformationsMemory[i][m_renderer.getCurrentImageIndex()], 0,
                    sizeof(transformation), 0, &data);
        memcpy(data, &transformation, sizeof(transformation));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformOutlineTransformationsMemory[i][m_renderer.getCurrentImageIndex()]);

        VulkanShaderDirectionalLight dirLight = {};
        dirLight.direction = m_camera.getViewMatrix() * glm::vec4(m_dirLightDirection, 0.0f);
        dirLight.ambient = m_dirLightColor * 0.3f;
        dirLight.diffuse = m_dirLightColor * 0.4f;
        dirLight.specular = m_dirLightColor * 0.3f;
        vkMapMemory(m_renderer.m_context.m_device,
                    m_uniformDirectionalLightBuffersMemory[m_renderer.getCurrentImageIndex()], 0, sizeof(dirLight), 0,
                    &data);
        memcpy(data, &dirLight, sizeof(dirLight));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformDirectionalLightBuffersMemory[m_renderer.getCurrentImageIndex()]);

        int lightIndex = 0;
        for (auto &light : m_pointLights)
        {
            if (light->isLit())
            {
                VulkanShaderPointLight pointLight = {};
                pointLight.position = m_camera.getViewMatrix() * glm::vec4(light->position, 1.0f);
                pointLight.ambient = light->color * 0.0f;
                pointLight.diffuse = light->color * 0.5f;
                pointLight.specular = light->color;
                pointLight.constant = 1.0f;
                pointLight.linear = 0.09f;
                pointLight.quadratic = 0.032f;
                vkMapMemory(m_renderer.m_context.m_device,
                            m_uniformPointLightBuffersMemory[m_renderer.getCurrentImageIndex()],
                            lightIndex * sizeof(pointLight), sizeof(pointLight), 0, &data);
                memcpy(data, &pointLight, sizeof(pointLight));
                vkUnmapMemory(m_renderer.m_context.m_device,
                              m_uniformPointLightBuffersMemory[m_renderer.getCurrentImageIndex()]);
                lightIndex++;
            }
        }

        VulkanShaderSpotLight spotLight = {};
        spotLight.position = glm::vec3(0);
        spotLight.direction = glm::vec3(0.0f, 0.0f, -1.0f);
        spotLight.ambient = m_spotLightColor * 0.2f;
        spotLight.diffuse = m_spotLightColor * 0.5f;
        spotLight.specular = m_spotLightColor;
        spotLight.innerCutOff = glm::cos(glm::radians(12.5f));
        spotLight.outerCutOff = glm::cos(glm::radians(14.5f));
        vkMapMemory(m_renderer.m_context.m_device, m_uniformSpotLightBuffersMemory[m_renderer.getCurrentImageIndex()],
                    0, sizeof(spotLight), 0, &data);
        memcpy(data, &spotLight, sizeof(spotLight));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformSpotLightBuffersMemory[m_renderer.getCurrentImageIndex()]);

        Material material = {};
        material.shininess = 32.0f;
        vkMapMemory(m_renderer.m_context.m_device, m_uniformMaterialBuffersMemory[i][m_renderer.getCurrentImageIndex()],
                    0, sizeof(material), 0, &data);
        memcpy(data, &material, sizeof(material));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformMaterialBuffersMemory[i][m_renderer.getCurrentImageIndex()]);
    }

    VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
    m_renderer.render(commandBuffer);
}

void VulkanLayerRenderer3DLights::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    LayerRenderer::addDrawable(drawable);

    Cube *cube = dynamic_cast<Cube *>(drawable.get());
    if (m_isAttached)
    {
        if (m_uniformTransformations.size() < m_drawables.size())
        {
            m_renderer.addUniformBuffers(sizeof(Transformation3DLights), m_uniformTransformations,
                                         m_uniformTransformationsMemory);
            m_pipeline->createSingleTransformationDescriptorSet(m_transformationDescriptorSets,
                                                                m_uniformTransformations.back());
            m_renderer.addUniformBuffers(sizeof(Transformation3DOutline), m_uniformOutlineTransformations,
                                         m_uniformOutlineTransformationsMemory);
            m_pipeline->createSingleTransformationDescriptorSet(m_outlineTransformationDescriptorSets,
                                                                m_uniformOutlineTransformations.back());
            m_renderer.addUniformBuffers(sizeof(Material), m_uniformMaterialBuffers, m_uniformMaterialBuffersMemory);
            m_pipeline->createSingleMaterialDescriptorSet(m_materialDescriptorSets, m_uniformMaterialBuffers.back());
        }
        if (cube->getType() == CubeType::TexturedCude)
        {
            VulkanTexturedCube *texturedCube = dynamic_cast<VulkanTexturedCube *>(cube);
            if (m_textureDescriptorOffset.find(texturedCube->getTexture().getName()) == m_textureDescriptorOffset.end())
            {
                const VulkanTexture &texture = texturedCube->getTexture();
                const VulkanTexture &specularTexture = texturedCube->getSpecularTexture();
                m_textureDescriptorOffset[texturedCube->getTexture().getName()] =
                    static_cast<unsigned int>(m_textureDescriptorSets.size());
                m_pipeline->createSingleTextureDescriptorSet(
                    m_textureDescriptorSets, texture.getImage().getImageView(), texture.getSampler().getSampler(),
                    specularTexture.getImage().getImageView(), specularTexture.getSampler().getSampler());
            }
        }
        m_isRecreationNeeded = true;
    }
}

void VulkanLayerRenderer3DLights::removeDrawable(Drawable *drawable)
{
    m_toBeRemoved.push_back(drawable);
    m_isRecreationNeeded = true;
}

void VulkanLayerRenderer3DLights::setDirLight(glm::vec3 direction, glm::vec3 color)
{
    m_dirLightDirection = direction;
    m_dirLightColor = color;
}

void VulkanLayerRenderer3DLights::addPointLight(VulkanPointLight &pointLight)
{
    m_pointLights.push_back(&pointLight);
}

void VulkanLayerRenderer3DLights::removePointLight(VulkanPointLight &pointLight)
{
    auto found_it =
        std::find_if(m_pointLights.begin(), m_pointLights.end(), [&](VulkanPointLight *p) { return p == &pointLight; });

    SPARK_CORE_ASSERT(found_it != m_pointLights.end(), "Tried to remove a point light which isn't in the layer");
    if (found_it != m_pointLights.end())
    {
        m_pointLights.erase(found_it);
    }
}

void VulkanLayerRenderer3DLights::setSpotLight(glm::vec3 color)
{
    m_spotLightColor = color;
}

void VulkanLayerRenderer3DLights::setWireframe(WireframeState state, glm::vec3 color)
{
    m_wireframe = state;
    m_wireframeColor = color;
    m_isRecreationNeeded = true;
}

void VulkanLayerRenderer3DLights::createCommandBuffers()
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

        struct Vulkan3DLightsConsts pushConsts = {};
        pushConsts.numberOfPointLights = 0;

        for (auto &light : m_pointLights)
        {
            if (light->isLit())
            {
                pushConsts.numberOfPointLights++;
            }
        }

        struct Vulkan3DOutlinePushConsts outlinePushConsts = {};
        VkClearAttachment clearAttachment = {};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue = clearValues[1];
        VkClearRect clearRect = {};
        clearRect.rect = m_framebuffer->getDepthImageDimentions();
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (m_wireframe != WireframeState::Only)
        {
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                Drawable *drawable = m_drawables[j].get();
                Cube *cube = dynamic_cast<Cube *>(drawable);
                if (cube->getType() == CubeType::TexturedCude)
                {
                    VulkanTexturedCube *texturedCube = dynamic_cast<VulkanTexturedCube *>(cube);
                    pushConsts.calcLight = true;
                    m_pipeline->bind(
                        commandBuffer, m_transformationDescriptorSets[j][i], m_lightsDescriptorSets[0][i],
                        m_materialDescriptorSets[j][i],
                        m_textureDescriptorSets[m_textureDescriptorOffset[texturedCube->getTexture().getName()]][i],
                        pushConsts);
                    if (texturedCube->isHighlighted())
                    {
                        vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
                    }
                    texturedCube->fillCommandBuffer(commandBuffer);
                    if (texturedCube->isHighlighted())
                    {
                        outlinePushConsts.color = texturedCube->getHighlightColor();
                        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                outlinePushConsts);
                        texturedCube->fillCommandBuffer(commandBuffer);
                    }
                }
                else if (cube->getType() == CubeType::ColoredCube)
                {
                    VulkanColoredCube *coloredCube = dynamic_cast<VulkanColoredCube *>(cube);
                    auto pointLight =
                        std::find_if(m_pointLights.begin(), m_pointLights.end(),
                                     [&drawable](const VulkanPointLight *x) { return x->drawable.get() == drawable; });
                    if (pointLight != m_pointLights.end() && (*pointLight)->isLit())
                    {
                        pushConsts.calcLight = false;
                        pushConsts.color = (*pointLight)->color;
                    }
                    else
                    {
                        pushConsts.calcLight = true;
                        pushConsts.color = coloredCube->getColor();
                    }
                    if (coloredCube->isHighlighted())
                    {
                        vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
                    }
                    m_pipeline->bind(commandBuffer, m_transformationDescriptorSets[j][i], m_lightsDescriptorSets[0][i],
                                     m_materialDescriptorSets[j][i], pushConsts);
                    coloredCube->fillCommandBuffer(commandBuffer);
                    if (coloredCube->isHighlighted())
                    {
                        outlinePushConsts.color = coloredCube->getHighlightColor();
                        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                outlinePushConsts);
                        coloredCube->fillCommandBuffer(commandBuffer);
                    }
                }
            }
        }

        if (m_wireframe == WireframeState::Both || m_wireframe == WireframeState::Only)
        {
            struct Vulkan3DWireframePushConsts wireframePushConsts = {};
            wireframePushConsts.color = m_wireframeColor;
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[j].get());
                m_wireframePipeline->bind(commandBuffer, m_transformationDescriptorSets[j][i], wireframePushConsts);
                drawable->fillCommandBuffer(commandBuffer);
            }
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
        i++;
    }
}
} // namespace Spark
