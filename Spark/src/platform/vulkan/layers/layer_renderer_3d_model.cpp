#include "layer_renderer_3d_model.h"
#include "platform/vulkan/drawables/colored_drawable.h"
#include "platform/vulkan/drawables/model.h"
#include "platform/vulkan/drawables/textured_drawable.h"
#include "platform/vulkan/drawables/vulkan_drawable.h"
#include "platform/vulkan/resource/mesh.h"
#include "platform/vulkan/resource/texture.h"

#include "spark/core/log.h"

#include <algorithm>

namespace Spark::Render
{
VulkanLayerRenderer3DModel::VulkanLayerRenderer3DModel(VulkanRenderer &renderer, Camera &camera)
    : m_renderer(renderer)
    , m_framebuffer(nullptr)
    , m_pipeline(nullptr)
    , m_uniformTransformations()
    , m_uniformTransformationsMemory()
    , m_transformationDescriptorSets()
    , m_outlineTransformationDescriptorSets()
    , m_uniformDirectionalLightBuffers()
    , m_uniformDirectionalLightBuffersMemory()
    , m_uniformPointLightBuffers()
    , m_uniformPointLightBuffersMemory()
    , m_uniformSpotLightBuffers()
    , m_uniformSpotLightBuffersMemory()
    , m_lightsDescriptorSets()
    , m_uniformMaterialBuffers()
    , m_uniformMaterialBuffersMemory()
    , m_materialDescriptorSets()
    , m_textureDescriptorSets()
    , m_textureDescriptorOffset()
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
        OnDetach();
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
    for (size_t i = 0; i < m_textureDescriptorSets.size(); i++)
    {
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_textureDescriptorSets[i].size(), m_textureDescriptorSets[i].data());
    }

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

    void *data;

    for (size_t i = 0; i < m_drawables.size(); i++)
    {
        VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[i].get());
        struct Transformation3D transformation = {};
        transformation.model = drawable->getTransformation();
        transformation.view = m_camera.getViewMatrix();
        transformation.projection = glm::perspective(m_camera.getZoom(),
                                                     m_renderer.m_context.m_swapChainExtent.width /
                                                         (float)m_renderer.m_context.m_swapChainExtent.height,
                                                     0.1f, 100.0f);
        transformation.projection[1][1] *= -1;
        vkMapMemory(m_renderer.m_context.m_device,
                    m_uniformTransformationsMemory[drawable][m_renderer.getCurrentImageIndex()], 0,
                    sizeof(transformation), 0, &data);
        memcpy(data, &transformation, sizeof(transformation));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformTransformationsMemory[drawable][m_renderer.getCurrentImageIndex()]);

        if (drawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *modelDrawable = dynamic_cast<VulkanDrawableModel *>(m_drawables[i].get());
            for (size_t j = 0; j < modelDrawable->getModel().getMeshes().size(); j++)
            {
                const Mesh *mesh = modelDrawable->getModel().getMeshes()[j].get();
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
                            m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0,
                            sizeof(material), 0, &data);
                memcpy(data, &material, sizeof(material));
                vkUnmapMemory(m_renderer.m_context.m_device,
                              m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
            }
        }
        else if (drawable->getDrawableType() == VulkanDrawableType::Textured)
        {
            MaterialModel material = {};
            material.baseColorDiffuse = {0, 0, 0};
            material.baseColorSpecular = {0, 0, 0};
            material.baseColorAmbient = {0, 0, 0};
            material.texturesAmount = 1;
            material.specularAmount = 1;
            material.shininess = 32.0f;
            const VulkanRenderPrimitive *primitive = drawable->getRenderPrimitives()[0];
            vkMapMemory(m_renderer.m_context.m_device,
                        m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0,
                        sizeof(material), 0, &data);
            memcpy(data, &material, sizeof(material));
            vkUnmapMemory(m_renderer.m_context.m_device,
                          m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
        }
        else if (drawable->getDrawableType() == VulkanDrawableType::Colored)
        {
            VulkanColoredDrawable *coloredDrawable = dynamic_cast<VulkanColoredDrawable *>(m_drawables[i].get());
            MaterialModel material = {};
            auto pointLight =
                std::find_if(m_pointLights.begin(), m_pointLights.end(), [&drawable](VulkanPointLight *x) {
                    return x->getDrawable().get() == dynamic_cast<Drawable3D *>(drawable);
                });
            if (pointLight != m_pointLights.end() && (*pointLight)->isLit())
            {
                material.pureColor = (*pointLight)->getColor();
            }
            else
            {
                material.baseColorDiffuse = coloredDrawable->getColor();
                material.baseColorSpecular = coloredDrawable->getColor();
                material.baseColorAmbient = coloredDrawable->getColor();
            }
            material.texturesAmount = 0;
            material.specularAmount = 0;
            material.shininess = 32.0f;
            const VulkanRenderPrimitive *primitive = drawable->getRenderPrimitives()[0];
            vkMapMemory(m_renderer.m_context.m_device,
                        m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()], 0,
                        sizeof(material), 0, &data);
            memcpy(data, &material, sizeof(material));
            vkUnmapMemory(m_renderer.m_context.m_device,
                          m_uniformMaterialBuffersMemory[primitive][m_renderer.getCurrentImageIndex()]);
        }
    }
    VulkanShaderDirectionalLightModel dirLight = {};
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
            VulkanShaderPointLightModel pointLight = {};
            pointLight.position = m_camera.getViewMatrix() * glm::vec4(light->getPosition(), 1.0f);
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

    VkCommandBuffer commandBuffer = m_commandBuffers[m_renderer.getCurrentImageIndex()];
    m_renderer.render(commandBuffer);
}

void VulkanLayerRenderer3DModel::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    LayerRenderer::addDrawable(drawable);

    if (m_isAttached)
    {
        std::vector<std::shared_ptr<Drawable>> drawables({drawable});
        createResourcesForDrawables(drawables);
        m_isRecreationNeeded = true;
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
            vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[j].get());
                std::vector<const VulkanRenderPrimitive *> primitives = drawable->getRenderPrimitives();
                if (drawable->getDrawableType() == VulkanDrawableType::Textured)
                {
                    VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(drawable);
                    pushConsts.calcLight = true;
                    for (auto &primitive : drawable->getRenderPrimitives())
                    {
                        drawPrimitive(drawable, primitive, commandBuffer, i, pushConsts,
                                      m_textureDescriptorOffset[texturedDrawable->getTexture().getName()]);
                    }
                }
                else if (drawable->getDrawableType() == VulkanDrawableType::Colored)
                {
                    VulkanColoredDrawable *coloredDrawable = dynamic_cast<VulkanColoredDrawable *>(drawable);
                    auto pointLight =
                        std::find_if(m_pointLights.begin(), m_pointLights.end(), [&drawable](VulkanPointLight *x) {
                            return x->getDrawable().get() == dynamic_cast<Drawable3D *>(drawable);
                        });
                    if (pointLight != m_pointLights.end() && (*pointLight)->isLit())
                    {
                        pushConsts.calcLight = false;
                    }
                    else
                    {
                        pushConsts.calcLight = true;
                    }
                    for (auto &primitive : drawable->getRenderPrimitives())
                    {
                        drawPrimitive(drawable, primitive, commandBuffer, i, pushConsts);
                    }
                }
                else if (drawable->getDrawableType() == VulkanDrawableType::Model)
                {
                    VulkanDrawableModel *modelDrawable = dynamic_cast<VulkanDrawableModel *>(m_drawables[j].get());
                    pushConsts.calcLight = true;
                    for (size_t k = 0; k < modelDrawable->getModel().getMeshes().size(); k++)
                    {
                        VulkanMesh *mesh =
                            reinterpret_cast<VulkanMesh *>(modelDrawable->getModel().getMeshes()[k].get());
                        drawPrimitive(drawable, mesh, commandBuffer, i, pushConsts,
                                      m_textureDescriptorOffset[mesh->getTexturesID() + mesh->getSpecularTexturesID()]);
                    }
                }
                else
                {
                    SPARK_CORE_ERROR("Drawable type not supported");
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
                                               struct Vulkan3DModelConsts &pushConsts,
                                               unsigned int textureDescriptorOffset)
{
    struct Vulkan3DOutlinePushConsts outlinePushConsts = {};
    outlinePushConsts.color = drawable->getHighlightColor();
    outlinePushConsts.lineWidth = drawable->getHighlightWidth();

    m_pipeline->bind(commandBuffer, m_transformationDescriptorSets[drawable][commandBufferIndex],
                     m_lightsDescriptorSets[0][commandBufferIndex],
                     m_materialDescriptorSets[primitive][commandBufferIndex],
                     m_textureDescriptorSets[textureDescriptorOffset][commandBufferIndex], pushConsts);
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
    m_renderer.createUniformBuffers(sizeof(VulkanShaderDirectionalLightModel), m_uniformDirectionalLightBuffers,
                                    m_uniformDirectionalLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderPointLightModel) * MAX_POINT_LIGHTS, m_uniformPointLightBuffers,
                                    m_uniformPointLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderSpotLightModel), m_uniformSpotLightBuffers,
                                    m_uniformSpotLightBuffersMemory);
    m_pipeline->createLightDescriptorSets(m_lightsDescriptorSets, m_uniformDirectionalLightBuffers,
                                          m_uniformPointLightBuffers, m_uniformSpotLightBuffers);
}

void VulkanLayerRenderer3DModel::destroyResourcesForLights()
{
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
}

void VulkanLayerRenderer3DModel::createResourcesForDrawables(std::vector<std::shared_ptr<Drawable>> &drawables)
{
    std::vector<std::vector<VkImageView>> textures;
    std::vector<std::vector<VkSampler>> samplers;
    std::vector<std::vector<VkImageView>> specularTextures;
    std::vector<std::vector<VkSampler>> specularSamplers;

    for (auto &drawable : drawables)
    {
        VulkanDrawable *vulkanDrawable = dynamic_cast<VulkanDrawable *>(drawable.get());
        SPARK_CORE_ASSERT(vulkanDrawable != nullptr, "Got a null drawble");
        createDrawableResources(drawable.get());
        if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Textured)
        {
            VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(vulkanDrawable);
            createResourcesForTexutredDrawable(*texturedDrawable, textures, samplers, specularTextures,
                                               specularSamplers);
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *drawableModel = dynamic_cast<VulkanDrawableModel *>(vulkanDrawable);
            createResourcesForModelDrawable(*drawableModel, textures, samplers, specularTextures, specularSamplers);
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Colored)
        {
            createPrimitiveResources(vulkanDrawable->getRenderPrimitives()[0]);
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

void VulkanLayerRenderer3DModel::createResourcesForTexutredDrawable(
    VulkanTexturedDrawable &drawable, std::vector<std::vector<VkImageView>> &textures,
    std::vector<std::vector<VkSampler>> &samplers, std::vector<std::vector<VkImageView>> &specularTextures,
    std::vector<std::vector<VkSampler>> &specularSamplers)
{
    if (m_textureDescriptorOffset.find(drawable.getTexture().getName()) == m_textureDescriptorOffset.end())
    {
        m_textureDescriptorOffset[drawable.getTexture().getName()] = (unsigned int)textures.size();
        textures.push_back({drawable.getTexture().getImage().getImageView()});
        samplers.push_back({drawable.getTexture().getSampler().getSampler()});
        specularTextures.push_back({drawable.getSpecularTexture().getImage().getImageView()});
        specularSamplers.push_back({drawable.getSpecularTexture().getSampler().getSampler()});
    }
    createPrimitiveResources(drawable.getRenderPrimitives()[0]);
}

void VulkanLayerRenderer3DModel::createResourcesForModelDrawable(
    VulkanDrawableModel &drawable, std::vector<std::vector<VkImageView>> &textures,
    std::vector<std::vector<VkSampler>> &samplers, std::vector<std::vector<VkImageView>> &specularTextures,
    std::vector<std::vector<VkSampler>> &specularSamplers)
{
    for (auto &mesh : drawable.getModel().getMeshes())
    {
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
        createPrimitiveResources(reinterpret_cast<VulkanMesh *>(mesh.get()));
    }
}

void VulkanLayerRenderer3DModel::createDrawableResources(const Drawable *drawable)
{
    m_uniformTransformations[drawable] = std::vector<VkBuffer>();
    m_uniformTransformationsMemory[drawable] = std::vector<VkDeviceMemory>();
    m_renderer.createUniformBuffers(sizeof(Transformation3D), m_uniformTransformations[drawable],
                                    m_uniformTransformationsMemory[drawable]);

    std::vector<std::vector<VkDescriptorSet>> descriptorSets;
    std::vector<std::vector<VkDescriptorSet>> outlineDescriptorSets;

    m_pipeline->createTransformationDescriptorSets(
        1, descriptorSets, std::vector<std::vector<VkBuffer>>({m_uniformTransformations[drawable]}));
    m_outlinePipeline->createTransformationDescriptorSets(
        1, outlineDescriptorSets, std::vector<std::vector<VkBuffer>>({m_uniformTransformations[drawable]}));

    m_transformationDescriptorSets[drawable] = descriptorSets[0];
    m_outlineTransformationDescriptorSets[drawable] = outlineDescriptorSets[0];
}

void VulkanLayerRenderer3DModel::destroyDrawableResources(const Drawable *drawable)
{
    vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                         (unsigned int)m_transformationDescriptorSets[drawable].size(),
                         m_transformationDescriptorSets[drawable].data());
    m_transformationDescriptorSets.erase(drawable);

    vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                         (unsigned int)m_outlineTransformationDescriptorSets[drawable].size(),
                         m_outlineTransformationDescriptorSets[drawable].data());
    m_outlineTransformationDescriptorSets.erase(drawable);

    for (size_t i = 0; i < m_uniformTransformations[drawable].size(); i++)
    {
        vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformTransformations[drawable][i], nullptr);
        vkFreeMemory(m_renderer.m_context.m_device, m_uniformTransformationsMemory[drawable][i], nullptr);
    }
    m_uniformTransformations.erase(drawable);
    m_uniformTransformationsMemory.erase(drawable);
}

void VulkanLayerRenderer3DModel::createPrimitiveResources(const VulkanRenderPrimitive *primitive)
{
    m_uniformMaterialBuffers[primitive] = std::vector<VkBuffer>();
    m_uniformMaterialBuffersMemory[primitive] = std::vector<VkDeviceMemory>();
    m_renderer.createUniformBuffers(sizeof(MaterialModel), m_uniformMaterialBuffers[primitive],
                                    m_uniformMaterialBuffersMemory[primitive]);

    std::vector<std::vector<VkDescriptorSet>> descriptorSets;

    m_pipeline->addMaterialDescriptorSets(descriptorSets,
                                          std::vector<std::vector<VkBuffer>>({m_uniformMaterialBuffers[primitive]}));

    m_materialDescriptorSets[primitive] = descriptorSets[0];
}

void VulkanLayerRenderer3DModel::destroyPrimitiveResources(const VulkanRenderPrimitive *primitive)
{
    vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                         (unsigned int)m_materialDescriptorSets[primitive].size(),
                         m_materialDescriptorSets[primitive].data());
    m_materialDescriptorSets.erase(primitive);

    for (size_t i = 0; i < m_uniformMaterialBuffers[primitive].size(); i++)
    {
        vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformMaterialBuffers[primitive][i], nullptr);
        vkFreeMemory(m_renderer.m_context.m_device, m_uniformMaterialBuffersMemory[primitive][i], nullptr);
    }
    m_uniformMaterialBuffers.erase(primitive);
    m_uniformMaterialBuffersMemory.erase(primitive);
}
} // namespace Spark::Render
