#include "layer_renderer_3d_model.h"
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/model.h"
#include "platform/vulkan/drawables/textured_drawable.h"
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/resource/mesh.h"
#include "platform/vulkan/resource/texture.h"

#include "spark/core/log.h"
#include "spark/resource/resource_manager.h"

#include <algorithm>

namespace Spark::Render
{
VulkanLayerRenderer3DModel::VulkanLayerRenderer3DModel(VulkanRenderer &renderer, Camera &camera, bool xRayHighlight)
    : m_renderer(renderer)
    , m_framebuffer(nullptr)
    , m_pipeline(nullptr)
    , m_outlinePipeline(nullptr)
    , m_cleanOutlinePipeline(nullptr)
    , m_wireframePipeline(nullptr)
    , m_isAttached(false)
    , m_isRecreationNeeded(false)
    , m_camera(&camera)
    , m_dirLightDirection({0.0f, 0.0f, 0.0f})
    , m_dirLightColor({1.0f, 1.0f, 1.0f})
    , m_spotLightColor({1.0f, 1.0f, 1.0f})
    , m_wireframe(WireframeState::None)
    , m_wireframeColor({0.0f, 0.0f, 0.0f})
    , m_xRayHighlight(xRayHighlight)
{
    m_framebuffer = reinterpret_cast<VulkanFramebuffer3D *>(renderer.createFramebuffer(VulkanFramebufferType::Type3D));
    m_pipeline = reinterpret_cast<VulkanPipeline3DModel *>(
        renderer.createPipeline(VulkanPipelineType::Type3DModel, *m_framebuffer));
    m_outlinePipeline = reinterpret_cast<VulkanPipeline3DOutline *>(
        renderer.createPipeline(VulkanPipelineType::Type3DOutline, *m_framebuffer));
    m_cleanOutlinePipeline = reinterpret_cast<VulkanPipeline3DOutline *>(
        renderer.createPipeline(VulkanPipelineType::Type3DCleanOutline, *m_framebuffer));
    m_wireframePipeline = reinterpret_cast<VulkanPipeline3DWireframe *>(
        renderer.createPipeline(VulkanPipelineType::Type3DWireframe, *m_framebuffer));
    m_commandBuffers.resize(renderer.getImagesAmount());
    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_commandBuffers[i] = renderer.m_context.createCommandBuffer();
    }
}

VulkanLayerRenderer3DModel::~VulkanLayerRenderer3DModel()
{
    if (m_isAttached)
    {
        VulkanLayerRenderer3DModel::OnDetach();
    }

    for (int i = 0; i < m_commandBuffers.size(); i++)
    {
        m_renderer.m_context.destroyCommandBuffer(m_commandBuffers[i]);
    }

    m_renderer.destroyPipeline(m_wireframePipeline);
    m_wireframePipeline = nullptr;
    m_renderer.destroyPipeline(m_cleanOutlinePipeline);
    m_cleanOutlinePipeline = nullptr;
    m_renderer.destroyPipeline(m_outlinePipeline);
    m_outlinePipeline = nullptr;
    m_renderer.destroyPipeline(m_pipeline);
    m_pipeline = nullptr;
    m_renderer.destroyFramebuffer(m_framebuffer);
    m_framebuffer = nullptr;
}

void VulkanLayerRenderer3DModel::OnAttach()
{
    createResourcesForLights();
    createResourcesForDrawables(m_drawables);
    createCommandBuffers();
    m_isAttached = true;
}

void VulkanLayerRenderer3DModel::OnDetach()
{
    m_renderer.waitForIdle();
    m_isAttached = false;
    for (VkCommandBuffer commandBuffer : m_commandBuffers)
    {
        m_renderer.resetCommandBuffer(commandBuffer);
    }

    m_textureDescriptorOffset.clear();
    m_textureMapping.clear();
    for (auto &set : m_textureDescriptorSets)
    {
        m_pipeline->destroyDescriptorSet(set);
    }
    m_textureDescriptorSets.clear();

    for (auto &drawable : m_drawables)
    {
        destroyResourcesForDrawable(drawable.get());
    }

    destroyResourcesForLights();
}

void VulkanLayerRenderer3DModel::OnRender()
{
    if (m_isRecreationNeeded || m_renderer.isRecreationNeeded())
    {
        for (VkCommandBuffer commandBuffer : m_commandBuffers)
        {
            m_renderer.resetCommandBuffer(commandBuffer);
        }

        while (!m_toBeRemoved.empty())
        {
            Drawable *drawableToRemove = m_toBeRemoved.back();
            destroyResourcesForDrawable(drawableToRemove);
            LayerRenderer::removeDrawable(drawableToRemove);
            m_toBeRemoved.pop_back();
        }

        createCommandBuffers();
        m_isRecreationNeeded = false;
    }

    for (auto &drawable : m_drawables)
    {
        updateDrawableData(drawable.get());
    }

    updateLightsData();

    VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
    m_renderer.render(commandBuffer);
}

void VulkanLayerRenderer3DModel::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    auto found_it =
        std::find_if(m_toBeRemoved.begin(), m_toBeRemoved.end(), [&](Drawable *p) { return p == drawable.get(); });
    if (found_it != m_toBeRemoved.end())
    {
        m_toBeRemoved.erase(found_it);
    }
    else
    {
        LayerRenderer::addDrawable(drawable);

        if (m_isAttached)
        {
            std::vector<std::shared_ptr<Drawable>> drawables({drawable});
            createResourcesForDrawables(drawables);
            m_isRecreationNeeded = true;
        }
    }
}

void VulkanLayerRenderer3DModel::removeDrawable(Drawable *drawable)
{
    m_toBeRemoved.push_back(drawable);
    m_isRecreationNeeded = true;
}

void VulkanLayerRenderer3DModel::setDirLight(glm::vec3 direction, glm::vec3 color)
{
    m_dirLightDirection = direction;
    m_dirLightColor = color;
}

void VulkanLayerRenderer3DModel::addPointLight(VulkanPointLight &pointLight)
{
    m_pointLights.push_back(&pointLight);
}

void VulkanLayerRenderer3DModel::removePointLight(VulkanPointLight &pointLight)
{
    auto found_it =
        std::find_if(m_pointLights.begin(), m_pointLights.end(), [&](VulkanPointLight *p) { return p == &pointLight; });

    SPARK_CORE_ASSERT(found_it != m_pointLights.end(), "Tried to remove a point light which isn't in the layer");
    if (found_it != m_pointLights.end())
    {
        m_pointLights.erase(found_it);
    }
}

void VulkanLayerRenderer3DModel::setSpotLight(glm::vec3 color)
{
    m_spotLightColor = color;
}

void VulkanLayerRenderer3DModel::setWireframe(WireframeState state, glm::vec3 color)
{
    m_wireframe = state;
    m_wireframeColor = color;
    m_isRecreationNeeded = true;
}

bool VulkanLayerRenderer3DModel::getXrayHighlight() const
{
    return m_xRayHighlight;
}

void VulkanLayerRenderer3DModel::setXrayHighlight(bool xRay)
{
    m_xRayHighlight = xRay;
    m_pipeline->setXrayHighlight(xRay);
    m_outlinePipeline->setXray(xRay);
    m_renderer.recreateSwapchain();
}

void VulkanLayerRenderer3DModel::setCamera(Render::Camera &camera)
{
    m_camera = &camera;
}

void VulkanLayerRenderer3DModel::createCommandBuffers()
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

        struct Vulkan3DModelConsts pushConsts = {};
        pushConsts.numberOfPointLights = 0;

        for (auto &light : m_pointLights)
        {
            if (light->isLit())
            {
                pushConsts.numberOfPointLights++;
            }
        }

        VkClearAttachment clearAttachment = {};
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue = clearValues[1];
        VkClearRect clearRect = {};
        clearRect.rect = m_framebuffer->getDepthImageDimentions();
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;

        if (m_wireframe != WireframeState::Only)
        {
            vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[j].get());
                Drawable3D *drawable3D = dynamic_cast<Drawable3D *>(m_drawables[j].get());
                std::vector<const VulkanRenderPrimitive *> primitives = drawable->getRenderPrimitives();
                pushConsts.calcLight = drawable3D->isCalculateLight();
                for (auto &primitive : drawable->getRenderPrimitives())
                {
                    drawPrimitive(drawable, primitive, commandBuffer, i, pushConsts);
                }
            }
        }

        if (m_wireframe == WireframeState::Only)
        {
            struct Vulkan3DWireframePushConsts wireframePushConsts = {};
            wireframePushConsts.color = m_wireframeColor;
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[j].get());
                for (auto &primitive : drawable->getRenderPrimitives())
                {
                    drawPrimitiveWireframe(drawable, primitive, commandBuffer, i, wireframePushConsts);
                }
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

void VulkanLayerRenderer3DModel::drawPrimitive(const VulkanDrawable *drawable, const VulkanRenderPrimitive *primitive,
                                               VkCommandBuffer commandBuffer, size_t commandBufferIndex,
                                               struct Vulkan3DModelConsts &pushConsts)
{
    struct Vulkan3DOutlinePushConsts outlinePushConsts = {};
    outlinePushConsts.color = drawable->getHighlightColor();
    outlinePushConsts.lineWidth = drawable->getHighlightWidth();

    m_pipeline->bind(commandBuffer, m_transformationDescriptorSets[drawable][commandBufferIndex],
                     m_lightsDescriptorSets[0][commandBufferIndex],
                     m_materialDescriptorSets[primitive][commandBufferIndex],
                     m_textureDescriptorSets[m_primitiveTextureOffset[primitive]][commandBufferIndex], pushConsts);
    primitive->fillCommandBuffer(commandBuffer);

    if (m_wireframe == WireframeState::Both)
    {
        struct Vulkan3DWireframePushConsts wireframePushConsts = {};
        wireframePushConsts.color = m_wireframeColor;
        m_wireframePipeline->bind(commandBuffer, m_transformationDescriptorSets[drawable][commandBufferIndex],
                                  wireframePushConsts);
        primitive->fillCommandBuffer(commandBuffer);
    }

    if (drawable->isHighlighted())
    {
        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[drawable][commandBufferIndex],
                                outlinePushConsts);
        primitive->fillCommandBuffer(commandBuffer);
    }

    m_cleanOutlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[drawable][commandBufferIndex],
                                 outlinePushConsts);
    primitive->fillCommandBuffer(commandBuffer);
}

void VulkanLayerRenderer3DModel::drawPrimitiveWireframe(const VulkanDrawable *drawable,
                                                        const VulkanRenderPrimitive *primitive,
                                                        VkCommandBuffer commandBuffer, size_t commandBufferIndex,
                                                        struct Vulkan3DWireframePushConsts &pushConsts)
{
    struct Vulkan3DOutlinePushConsts outlinePushConsts = {};
    outlinePushConsts.color = drawable->getHighlightColor();
    outlinePushConsts.lineWidth = drawable->getHighlightWidth();

    m_wireframePipeline->bind(commandBuffer, m_transformationDescriptorSets[drawable][commandBufferIndex], pushConsts);
    primitive->fillCommandBuffer(commandBuffer);

    if (drawable->isHighlighted())
    {
        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[drawable][commandBufferIndex],
                                outlinePushConsts);
        primitive->fillCommandBuffer(commandBuffer);
    }

    m_cleanOutlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[drawable][commandBufferIndex],
                                 outlinePushConsts);
    primitive->fillCommandBuffer(commandBuffer);
}

void VulkanLayerRenderer3DModel::createResourcesForLights()
{
    std::tie(m_uniformDirectionalLightBuffers, m_uniformDirectionalLightBuffersMemory) =
        m_renderer.createUniformBuffers(sizeof(VulkanShaderDirectionalLightModel));
    std::tie(m_uniformPointLightBuffers, m_uniformPointLightBuffersMemory) =
        m_renderer.createUniformBuffers(sizeof(VulkanShaderPointLightModel) * MAX_POINT_LIGHTS);
    std::tie(m_uniformSpotLightBuffers, m_uniformSpotLightBuffersMemory) =
        m_renderer.createUniformBuffers(sizeof(VulkanShaderSpotLightModel));
    m_pipeline->createLightDescriptorSets(m_lightsDescriptorSets, m_uniformDirectionalLightBuffers,
                                          m_uniformPointLightBuffers, m_uniformSpotLightBuffers);
}

void VulkanLayerRenderer3DModel::destroyResourcesForLights()
{
    for (auto &set : m_lightsDescriptorSets)
    {
        m_pipeline->destroyDescriptorSet(set);
    }
    m_lightsDescriptorSets.clear();

    m_renderer.destroyUniformBuffers(m_uniformSpotLightBuffers, m_uniformSpotLightBuffersMemory);
    m_renderer.destroyUniformBuffers(m_uniformPointLightBuffers, m_uniformPointLightBuffersMemory);
    m_renderer.destroyUniformBuffers(m_uniformDirectionalLightBuffers, m_uniformDirectionalLightBuffersMemory);
}

void VulkanLayerRenderer3DModel::createResourcesForDrawables(std::vector<std::shared_ptr<Drawable>> &drawables)
{
    std::vector<std::vector<VkImageView>> textures;
    std::vector<std::vector<VkSampler>> samplers;
    std::vector<std::vector<VkImageView>> specularTextures;
    std::vector<std::vector<VkSampler>> specularSamplers;

    if (m_textureDescriptorOffset.find(BLANK_TEXTURE_NAME) == m_textureDescriptorOffset.end())
    {
        const VulkanTexture *blankTexture =
            dynamic_cast<const VulkanTexture *>(ResourceManager::getTexture(BLANK_TEXTURE_NAME));
        SPARK_CORE_ASSERT(blankTexture != nullptr, "No blank texture, can't create reesources for drawables");
        m_textureDescriptorOffset[BLANK_TEXTURE_NAME] = (unsigned int)textures.size();
        textures.push_back({blankTexture->getImage().getImageView()});
        samplers.push_back({blankTexture->getSampler().getSampler()});
        specularTextures.push_back({blankTexture->getImage().getImageView()});
        specularSamplers.push_back({blankTexture->getSampler().getSampler()});
    }

    for (auto &drawable : drawables)
    {
        VulkanDrawable *vulkanDrawable = dynamic_cast<VulkanDrawable *>(drawable.get());
        SPARK_CORE_ASSERT(vulkanDrawable != nullptr, "Got a null drawble");
        createDrawableResources(drawable.get());
        if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Textured)
        {
            VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(vulkanDrawable);
            SPARK_CORE_ASSERT(texturedDrawable != nullptr, "Couldn't convert drawable to textured drawable");
            createResourcesForTexturedDrawable(*texturedDrawable, textures, samplers, specularTextures,
                                               specularSamplers);
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *drawableModel = dynamic_cast<VulkanDrawableModel *>(vulkanDrawable);
            SPARK_CORE_ASSERT(drawableModel != nullptr, "Couldn't convert drawable to model drawable");
            createResourcesForModelDrawable(*drawableModel, textures, samplers, specularTextures, specularSamplers);
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Colored)
        {
            const VulkanRenderPrimitive *primitive = vulkanDrawable->getRenderPrimitives()[0];
            m_primitiveTextureOffset[primitive] = m_textureDescriptorOffset[BLANK_TEXTURE_NAME];
            createPrimitiveResources(primitive);
        }
        else
        {
            SPARK_CORE_ERROR("Unknown drawable type");
            SPARK_DEBUG_BREAK();
        }
    }

    m_pipeline->addTextureDescriptorSets(m_textureDescriptorSets, textures, samplers, specularTextures,
                                         specularSamplers, static_cast<unsigned int>(textures.size()));
}

void VulkanLayerRenderer3DModel::destroyResourcesForDrawable(Drawable *drawable)
{
    VulkanDrawable *vulkanDrawable = dynamic_cast<VulkanDrawable *>(drawable);
    SPARK_CORE_ASSERT(vulkanDrawable != nullptr, "Got a null drawble");
    for (auto &primitive : vulkanDrawable->getRenderPrimitives())
    {
        destroyPrimitiveResources(primitive);
    }

    destroyDrawableResources(drawable);
}

void VulkanLayerRenderer3DModel::createResourcesForTexturedDrawable(
    VulkanTexturedDrawable &drawable, std::vector<std::vector<VkImageView>> &textures,
    std::vector<std::vector<VkSampler>> &samplers, std::vector<std::vector<VkImageView>> &specularTextures,
    std::vector<std::vector<VkSampler>> &specularSamplers)
{
    const VulkanRenderPrimitive *primitive = drawable.getRenderPrimitives()[0];
    if (m_textureDescriptorOffset.find(drawable.getTexture().getName()) == m_textureDescriptorOffset.end())
    {
        m_textureDescriptorOffset[drawable.getTexture().getName()] = (unsigned int)textures.size();
        textures.push_back({drawable.getTexture().getImage().getImageView()});
        samplers.push_back({drawable.getTexture().getSampler().getSampler()});
        specularTextures.push_back({drawable.getSpecularTexture().getImage().getImageView()});
        specularSamplers.push_back({drawable.getSpecularTexture().getSampler().getSampler()});
    }
    m_primitiveTextureOffset[primitive] = m_textureDescriptorOffset[drawable.getTexture().getName()];
    createPrimitiveResources(primitive);
}

void VulkanLayerRenderer3DModel::createResourcesForModelDrawable(
    VulkanDrawableModel &drawable, std::vector<std::vector<VkImageView>> &textures,
    std::vector<std::vector<VkSampler>> &samplers, std::vector<std::vector<VkImageView>> &specularTextures,
    std::vector<std::vector<VkSampler>> &specularSamplers)
{
    for (auto &mesh : drawable.getModel().getMeshes())
    {
        const VulkanRenderPrimitive *primitive = reinterpret_cast<VulkanMesh *>(mesh.get());
        if (m_textureDescriptorOffset.find(mesh->getTexturesID() + mesh->getSpecularTexturesID()) ==
            m_textureDescriptorOffset.end())
        {
            std::vector<VkImageView> meshTextures;
            std::vector<VkSampler> meshSamplers;
            std::vector<VkImageView> meshSpecularTextures;
            std::vector<VkSampler> meshSpecularSamplers;
            m_textureDescriptorOffset[mesh->getTexturesID() + mesh->getSpecularTexturesID()] =
                (unsigned int)textures.size();
            for (auto &texture : mesh->getTextures())
            {
                const VulkanTexture *vulkanTexture = reinterpret_cast<const VulkanTexture *>(texture);
                meshTextures.push_back(vulkanTexture->getImage().getImageView());
                meshSamplers.push_back(vulkanTexture->getSampler().getSampler());
            }
            for (auto &texture : mesh->getSpecularTextures())
            {
                const VulkanTexture *vulkanTexture = reinterpret_cast<const VulkanTexture *>(texture);
                meshSpecularTextures.push_back(vulkanTexture->getImage().getImageView());
                meshSpecularSamplers.push_back(vulkanTexture->getSampler().getSampler());
            }
            textures.push_back(meshTextures);
            samplers.push_back(meshSamplers);
            specularTextures.push_back(meshTextures);
            specularSamplers.push_back(meshSamplers);
        }
        m_primitiveTextureOffset[primitive] =
            m_textureDescriptorOffset[mesh->getTexturesID() + mesh->getSpecularTexturesID()];
        createPrimitiveResources(primitive);
    }
}

void VulkanLayerRenderer3DModel::createDrawableResources(const Drawable *drawable)
{
    std::tie(m_uniformTransformations[drawable], m_uniformTransformationsMemory[drawable]) =
        m_renderer.createUniformBuffers(sizeof(Transformation3D));

    m_transformationDescriptorSets[drawable] =
        m_pipeline->createTransformationDescriptorSet(m_uniformTransformations[drawable]);
    m_outlineTransformationDescriptorSets[drawable] =
        m_outlinePipeline->createTransformationDescriptorSet(m_uniformTransformations[drawable]);
}

void VulkanLayerRenderer3DModel::destroyDrawableResources(const Drawable *drawable)
{
    m_pipeline->destroyDescriptorSet(m_transformationDescriptorSets[drawable]);
    m_transformationDescriptorSets.erase(drawable);

    m_pipeline->destroyDescriptorSet(m_outlineTransformationDescriptorSets[drawable]);
    m_outlineTransformationDescriptorSets.erase(drawable);

    m_renderer.destroyUniformBuffers(m_uniformTransformations[drawable], m_uniformTransformationsMemory[drawable]);
    m_uniformTransformations.erase(drawable);
    m_uniformTransformationsMemory.erase(drawable);
}

void VulkanLayerRenderer3DModel::createPrimitiveResources(const VulkanRenderPrimitive *primitive)
{
    std::tie(m_uniformMaterialBuffers[primitive], m_uniformMaterialBuffersMemory[primitive]) =
        m_renderer.createUniformBuffers(sizeof(MaterialModel));

    m_materialDescriptorSets[primitive] = m_pipeline->createMaterialDescriptorSet(m_uniformMaterialBuffers[primitive]);
}

void VulkanLayerRenderer3DModel::destroyPrimitiveResources(const VulkanRenderPrimitive *primitive)
{
    m_pipeline->destroyDescriptorSet(m_materialDescriptorSets[primitive]);
    m_materialDescriptorSets.erase(primitive);

    m_renderer.destroyUniformBuffers(m_uniformMaterialBuffers[primitive], m_uniformMaterialBuffersMemory[primitive]);
    m_uniformMaterialBuffers.erase(primitive);
    m_uniformMaterialBuffersMemory.erase(primitive);
}

void VulkanLayerRenderer3DModel::updateLightsData()
{
    updateDirLightData();
    updatePointLightsData();
    updateSpotLightData();
}

void VulkanLayerRenderer3DModel::updateDirLightData()
{
    void *data = nullptr;
    VulkanShaderDirectionalLightModel dirLight = {};
    dirLight.direction = m_camera->getViewMatrix() * glm::vec4(m_dirLightDirection, 0.0f);
    dirLight.ambient = m_dirLightColor * 0.3f;
    dirLight.diffuse = m_dirLightColor * 0.4f;
    dirLight.specular = m_dirLightColor * 0.3f;
    vkMapMemory(m_renderer.m_context.m_device,
                m_uniformDirectionalLightBuffersMemory[m_renderer.getCurrentImageIndex()], 0, sizeof(dirLight), 0,
                &data);
    memcpy(data, &dirLight, sizeof(dirLight));
    vkUnmapMemory(m_renderer.m_context.m_device,
                  m_uniformDirectionalLightBuffersMemory[m_renderer.getCurrentImageIndex()]);
}

void VulkanLayerRenderer3DModel::updatePointLightsData()
{
    void *data = nullptr;
    int lightIndex = 0;
    for (auto &light : m_pointLights)
    {
        if (light->isLit())
        {
            VulkanShaderPointLightModel pointLight = {};
            pointLight.position = m_camera->getViewMatrix() * glm::vec4(light->getPosition(), 1.0f);
            pointLight.ambient = light->getColor() * 0.0f;
            pointLight.diffuse = light->getColor() * 0.5f;
            pointLight.specular = light->getColor();
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
}

void VulkanLayerRenderer3DModel::updateSpotLightData()
{
    void *data = nullptr;
    VulkanShaderSpotLightModel spotLight = {};
    spotLight.position = glm::vec3(0);
    spotLight.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    spotLight.ambient = m_spotLightColor * 0.2f;
    spotLight.diffuse = m_spotLightColor * 0.5f;
    spotLight.specular = m_spotLightColor;
    spotLight.innerCutOff = glm::cos(glm::radians(12.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(14.5f));
    vkMapMemory(m_renderer.m_context.m_device, m_uniformSpotLightBuffersMemory[m_renderer.getCurrentImageIndex()], 0,
                sizeof(spotLight), 0, &data);
    memcpy(data, &spotLight, sizeof(spotLight));
    vkUnmapMemory(m_renderer.m_context.m_device, m_uniformSpotLightBuffersMemory[m_renderer.getCurrentImageIndex()]);
}

void VulkanLayerRenderer3DModel::updateDrawableData(const Drawable *drawable)
{
    void *data = nullptr;
    const VulkanDrawable *vulkanDrawable = dynamic_cast<const VulkanDrawable *>(drawable);
    SPARK_CORE_ASSERT(vulkanDrawable != nullptr, "Got a null drawble");

    struct Transformation3D transformation = {};
    transformation.model = drawable->getTransformation();
    transformation.view = m_camera->getViewMatrix();
    transformation.projection = glm::perspective(m_camera->getZoomRadians(),
                                                 m_renderer.m_context.m_swapChainExtent.width /
                                                     (float)m_renderer.m_context.m_swapChainExtent.height,
                                                 0.1f, 100.0f);
    transformation.projection[1][1] *= -1;
    vkMapMemory(m_renderer.m_context.m_device,
                m_uniformTransformationsMemory[drawable][m_renderer.getCurrentImageIndex()], 0, sizeof(transformation),
                0, &data);
    memcpy(data, &transformation, sizeof(transformation));
    vkUnmapMemory(m_renderer.m_context.m_device,
                  m_uniformTransformationsMemory[drawable][m_renderer.getCurrentImageIndex()]);

    if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Colored)
    {
        updateColoredDrawableMaterialData(dynamic_cast<const VulkanColoredDrawable *>(vulkanDrawable));
    }
    else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Textured)
    {
        updateTexturedDrawableMaterialData(dynamic_cast<const VulkanTexturedDrawable *>(vulkanDrawable));
    }
    else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Model)
    {
        updateModelDrawableMaterialData(dynamic_cast<const VulkanDrawableModel *>(vulkanDrawable));
    }
}

void VulkanLayerRenderer3DModel::updateColoredDrawableMaterialData(const VulkanColoredDrawable *drawable)
{
    void *data;
    MaterialModel material = {};
    SPARK_CORE_ASSERT(drawable != nullptr, "Got a null drawble");
    auto pointLight = std::find_if(m_pointLights.begin(), m_pointLights.end(), [&drawable](VulkanPointLight *x) {
        return x->getDrawable().get() == dynamic_cast<const Drawable3D *>(drawable);
    });
    if (pointLight != m_pointLights.end() && (*pointLight)->isLit())
    {
        material.pureColor = (*pointLight)->getColor();
    }
    else
    {
        material.pureColor = drawable->getColor();
        material.baseColorDiffuse = drawable->getColor();
        material.baseColorSpecular = drawable->getColor();
        material.baseColorAmbient = drawable->getColor();
    }
    material.texturesAmount = 0;
    material.specularAmount = 0;
    material.shininess = 32.0f;
    const VulkanRenderPrimitive *primitive = drawable->getRenderPrimitives()[0];
    vkMapMemory(m_renderer.m_context.m_device,
                m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0, sizeof(material), 0,
                &data);
    memcpy(data, &material, sizeof(material));
    vkUnmapMemory(m_renderer.m_context.m_device,
                  m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
}

void VulkanLayerRenderer3DModel::updateTexturedDrawableMaterialData(const VulkanTexturedDrawable *drawable)
{
    void *data = nullptr;
    SPARK_CORE_ASSERT(drawable != nullptr, "Got a null drawble");
    MaterialModel material = {};
    material.baseColorDiffuse = {0, 0, 0};
    material.baseColorSpecular = {0, 0, 0};
    material.baseColorAmbient = {0, 0, 0};
    material.texturesAmount = 1;
    material.specularAmount = 1;
    material.shininess = 32.0f;
    const VulkanRenderPrimitive *primitive = drawable->getRenderPrimitives()[0];
    vkMapMemory(m_renderer.m_context.m_device,
                m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0, sizeof(material), 0,
                &data);
    memcpy(data, &material, sizeof(material));
    vkUnmapMemory(m_renderer.m_context.m_device,
                  m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
}

void VulkanLayerRenderer3DModel::updateModelDrawableMaterialData(const VulkanDrawableModel *drawable)
{
    void *data = nullptr;
    SPARK_CORE_ASSERT(drawable != nullptr, "Got a null drawble");
    for (size_t j = 0; j < drawable->getModel().getMeshes().size(); j++)
    {
        const Mesh *mesh = drawable->getModel().getMeshes()[j].get();
        const VulkanRenderPrimitive *primitive = reinterpret_cast<const VulkanMesh *>(mesh);
        MaterialModel material = {};
        struct MeshBaseColor baseColor = mesh->getBaseColor();
        material.baseColorDiffuse = baseColor.diffuse;
        material.baseColorSpecular = baseColor.specular;
        material.baseColorAmbient = baseColor.ambient;
        material.texturesAmount = static_cast<unsigned int>(mesh->getTextures().size());
        material.specularAmount = static_cast<unsigned int>(mesh->getSpecularTextures().size());
        material.shininess = mesh->getShininess();
        vkMapMemory(m_renderer.m_context.m_device,
                    m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0, sizeof(material),
                    0, &data);
        memcpy(data, &material, sizeof(material));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
    }
}
} // namespace Spark::Render
