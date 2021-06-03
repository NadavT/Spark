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
    std::vector<std::vector<VkImageView>> textures;
    std::vector<std::vector<VkSampler>> samplers;
    std::vector<std::vector<VkImageView>> specularTextures;
    std::vector<std::vector<VkSampler>> specularSamplers;
    unsigned int materialsAmount = 0;
    m_renderer.createUniformBuffers(sizeof(Transformation3DModel), m_uniformTransformations,
                                    m_uniformTransformationsMemory, (unsigned int)m_drawables.size());
    m_pipeline->createTransformationDescriptorSets((unsigned int)m_drawables.size(), m_transformationDescriptorSets,
                                                   m_uniformTransformations);
    m_renderer.createUniformBuffers(sizeof(Transformation3DOutline), m_uniformOutlineTransformations,
                                    m_uniformOutlineTransformationsMemory, (unsigned int)m_drawables.size());
    m_outlinePipeline->createTransformationDescriptorSets(
        (unsigned int)m_drawables.size(), m_outlineTransformationDescriptorSets, m_uniformOutlineTransformations);

    m_renderer.createUniformBuffers(sizeof(VulkanShaderDirectionalLightModel), m_uniformDirectionalLightBuffers,
                                    m_uniformDirectionalLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderPointLightModel) * MAX_POINT_LIGHTS, m_uniformPointLightBuffers,
                                    m_uniformPointLightBuffersMemory);
    m_renderer.createUniformBuffers(sizeof(VulkanShaderSpotLightModel), m_uniformSpotLightBuffers,
                                    m_uniformSpotLightBuffersMemory);
    m_pipeline->createLightDescriptorSets(m_lightsDescriptorSets, m_uniformDirectionalLightBuffers,
                                          m_uniformPointLightBuffers, m_uniformSpotLightBuffers);

    for (auto drawable : m_drawables)
    {
        VulkanDrawable *vulkanDrawable = dynamic_cast<VulkanDrawable *>(drawable.get());
        if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Textured)
        {
            VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(vulkanDrawable);
            if (m_textureDescriptorOffset.find(texturedDrawable->getTexture().getName()) ==
                m_textureDescriptorOffset.end())
            {
                m_textureDescriptorOffset[texturedDrawable->getTexture().getName()] = (unsigned int)textures.size();
                textures.push_back({texturedDrawable->getTexture().getImage().getImageView()});
                samplers.push_back({texturedDrawable->getTexture().getSampler().getSampler()});
                specularTextures.push_back({texturedDrawable->getSpecularTexture().getImage().getImageView()});
                specularSamplers.push_back({texturedDrawable->getSpecularTexture().getSampler().getSampler()});
            }
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *drawableModel = dynamic_cast<VulkanDrawableModel *>(vulkanDrawable);
            for (auto &mesh : drawableModel->getModel().getMeshes())
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
                    for (auto texture : mesh->getTextures())
                    {
                        const VulkanTexture *vulkanTexture = reinterpret_cast<const VulkanTexture *>(texture);
                        meshTextures.push_back(vulkanTexture->getImage().getImageView());
                        meshSamplers.push_back(vulkanTexture->getSampler().getSampler());
                    }
                    for (auto texture : mesh->getSpecularTextures())
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
            }
            materialsAmount = static_cast<unsigned int>(drawableModel->getModel().getMeshes().size());
        }
        m_uniformMaterialBuffers.push_back(std::vector<std::vector<VkBuffer>>());
        m_uniformMaterialBuffersMemory.push_back(std::vector<std::vector<VkDeviceMemory>>());
        m_renderer.createUniformBuffers(sizeof(MaterialModel), m_uniformMaterialBuffers.back(),
                                        m_uniformMaterialBuffersMemory.back(), materialsAmount);
        m_materialDescriptorSets.push_back(std::vector<std::vector<VkDescriptorSet>>());
        m_pipeline->createMaterialDescriptorSets(materialsAmount, m_materialDescriptorSets.back(),
                                                 m_uniformMaterialBuffers.back());
    }

    m_pipeline->createTextureDescriptorSets(m_textureDescriptorSets, textures, samplers, specularTextures,
                                            specularSamplers);
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
        for (size_t j = 0; j < m_uniformMaterialBuffers[i].size(); j++)
        {
            vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                                 (unsigned int)m_materialDescriptorSets[i][j].size(),
                                 m_materialDescriptorSets[i][j].data());
        }
        vkFreeDescriptorSets(m_renderer.m_context.m_device, m_renderer.m_context.m_descriptorPool,
                             (unsigned int)m_outlineTransformationDescriptorSets[i].size(),
                             m_outlineTransformationDescriptorSets[i].data());
        for (size_t j = 0; j < m_uniformMaterialBuffers[i].size(); j++)
        {
            for (size_t k = 0; k < m_uniformMaterialBuffers[i][j].size(); k++)
            {
                vkDestroyBuffer(m_renderer.m_context.m_device, m_uniformMaterialBuffers[i][j][k], nullptr);
                vkFreeMemory(m_renderer.m_context.m_device, m_uniformMaterialBuffersMemory[i][j][k], nullptr);
            }
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
            LayerRenderer::removeDrawable(m_toBeRemoved.back());
            m_toBeRemoved.pop_back();
        }

        createCommandBuffers();
        m_isRecreationNeeded = false;
    }

    void *data;

    for (size_t i = 0; i < m_drawables.size(); i++)
    {
        VulkanDrawable *drawable = dynamic_cast<VulkanDrawable *>(m_drawables[i].get());
        struct Transformation3DModel transformation = {};
        transformation.model = drawable->getTransformation();
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

        // transformation.model = glm::scale(transformation.model, glm::vec3(1.1, 1.1, 1.1));
        vkMapMemory(m_renderer.m_context.m_device,
                    m_uniformOutlineTransformationsMemory[i][m_renderer.getCurrentImageIndex()], 0,
                    sizeof(transformation), 0, &data);
        memcpy(data, &transformation, sizeof(transformation));
        vkUnmapMemory(m_renderer.m_context.m_device,
                      m_uniformOutlineTransformationsMemory[i][m_renderer.getCurrentImageIndex()]);

        if (drawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *modelDrawable = dynamic_cast<VulkanDrawableModel *>(m_drawables[i].get());
            for (size_t j = 0; j < modelDrawable->getModel().getMeshes().size(); j++)
            {
                const Mesh *mesh = modelDrawable->getModel().getMeshes()[j].get();
                MaterialModel material = {};
                struct MeshBaseColor baseColor = mesh->getBaseColor();
                material.baseColorDiffuse = baseColor.diffuse;
                material.baseColorSpecular = baseColor.specular;
                material.baseColorAmbient = baseColor.ambient;
                material.texturesAmount = static_cast<unsigned int>(mesh->getTextures().size());
                material.specularAmount = static_cast<unsigned int>(mesh->getSpecularTextures().size());
                material.shininess = mesh->getShininess();
                vkMapMemory(m_renderer.m_context.m_device,
                            m_uniformMaterialBuffersMemory[i][j][m_renderer.getCurrentImageIndex()], 0,
                            sizeof(material), 0, &data);
                memcpy(data, &material, sizeof(material));
                vkUnmapMemory(m_renderer.m_context.m_device,
                              m_uniformMaterialBuffersMemory[i][j][m_renderer.getCurrentImageIndex()]);
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
            vkMapMemory(m_renderer.m_context.m_device,
                        m_uniformMaterialBuffersMemory[i][0][m_renderer.getCurrentImageIndex()], 0, sizeof(material), 0,
                        &data);
            memcpy(data, &material, sizeof(material));
            vkUnmapMemory(m_renderer.m_context.m_device,
                          m_uniformMaterialBuffersMemory[i][0][m_renderer.getCurrentImageIndex()]);
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
            vkMapMemory(m_renderer.m_context.m_device,
                        m_uniformMaterialBuffersMemory[i][0][m_renderer.getCurrentImageIndex()], 0, sizeof(material), 0,
                        &data);
            memcpy(data, &material, sizeof(material));
            vkUnmapMemory(m_renderer.m_context.m_device,
                          m_uniformMaterialBuffersMemory[i][0][m_renderer.getCurrentImageIndex()]);
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
    unsigned int materialsAmount = 1;

    VulkanDrawable *vulkanDrawable = dynamic_cast<VulkanDrawable *>(drawable.get());
    if (m_isAttached)
    {
        if (m_uniformTransformations.size() < m_drawables.size())
        {
            m_renderer.addUniformBuffers(sizeof(Transformation3DModel), m_uniformTransformations,
                                         m_uniformTransformationsMemory);
            m_pipeline->createSingleTransformationDescriptorSet(m_transformationDescriptorSets,
                                                                m_uniformTransformations.back());
            m_renderer.addUniformBuffers(sizeof(Transformation3DOutline), m_uniformOutlineTransformations,
                                         m_uniformOutlineTransformationsMemory);
            m_pipeline->createSingleTransformationDescriptorSet(m_outlineTransformationDescriptorSets,
                                                                m_uniformOutlineTransformations.back());
        }
        if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Textured)
        {
            VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(vulkanDrawable);
            if (m_textureDescriptorOffset.find(texturedDrawable->getTexture().getName()) ==
                m_textureDescriptorOffset.end())
            {
                m_textureDescriptorOffset[texturedDrawable->getTexture().getName()] =
                    (unsigned int)m_textureDescriptorOffset.size();
                m_pipeline->createSingleTextureDescriptorSet(
                    m_textureDescriptorSets, {texturedDrawable->getTexture().getImage().getImageView()},
                    {texturedDrawable->getTexture().getSampler().getSampler()},
                    {texturedDrawable->getSpecularTexture().getImage().getImageView()},
                    {texturedDrawable->getSpecularTexture().getSampler().getSampler()});
            }
        }
        else if (vulkanDrawable->getDrawableType() == VulkanDrawableType::Model)
        {
            VulkanDrawableModel *drawableModel = dynamic_cast<VulkanDrawableModel *>(vulkanDrawable);
            for (auto &mesh : drawableModel->getModel().getMeshes())
            {
                if (m_textureDescriptorOffset.find(mesh->getTexturesID() + mesh->getSpecularTexturesID()) ==
                    m_textureDescriptorOffset.end())
                {
                    std::vector<VkImageView> meshTextures;
                    std::vector<VkSampler> meshSamplers;
                    std::vector<VkImageView> meshSpecularTextures;
                    std::vector<VkSampler> meshSpecularSamplers;
                    m_textureDescriptorOffset[mesh->getTexturesID() + mesh->getSpecularTexturesID()] =
                        (unsigned int)m_textureDescriptorOffset.size();
                    for (auto texture : mesh->getTextures())
                    {
                        const VulkanTexture *vulkanTexture = reinterpret_cast<const VulkanTexture *>(texture);
                        meshTextures.push_back(vulkanTexture->getImage().getImageView());
                        meshSamplers.push_back(vulkanTexture->getSampler().getSampler());
                    }
                    for (auto texture : mesh->getSpecularTextures())
                    {
                        const VulkanTexture *vulkanTexture = reinterpret_cast<const VulkanTexture *>(texture);
                        meshSpecularTextures.push_back(vulkanTexture->getImage().getImageView());
                        meshSpecularSamplers.push_back(vulkanTexture->getSampler().getSampler());
                    }
                    m_pipeline->createSingleTextureDescriptorSet(m_textureDescriptorSets, meshTextures, meshSamplers,
                                                                 meshSpecularTextures, meshSpecularSamplers);
                }
            }
            materialsAmount = static_cast<unsigned int>(drawableModel->getModel().getMeshes().size());
        }
        m_uniformMaterialBuffers.push_back(std::vector<std::vector<VkBuffer>>());
        m_uniformMaterialBuffersMemory.push_back(std::vector<std::vector<VkDeviceMemory>>());
        m_renderer.createUniformBuffers(sizeof(MaterialModel), m_uniformMaterialBuffers.back(),
                                        m_uniformMaterialBuffersMemory.back(), materialsAmount);
        m_materialDescriptorSets.push_back(std::vector<std::vector<VkDescriptorSet>>());
        m_pipeline->createMaterialDescriptorSets(materialsAmount, m_materialDescriptorSets.back(),
                                                 m_uniformMaterialBuffers.back());
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
                if (drawable->getDrawableType() == VulkanDrawableType::Textured)
                {
                    VulkanTexturedDrawable *texturedDrawable = dynamic_cast<VulkanTexturedDrawable *>(drawable);
                    pushConsts.calcLight = true;
                    m_pipeline->bind(
                        commandBuffer, m_transformationDescriptorSets[j][i], m_lightsDescriptorSets[0][i],
                        m_materialDescriptorSets[j][0][i],
                        m_textureDescriptorSets[m_textureDescriptorOffset[texturedDrawable->getTexture().getName()]][i],
                        pushConsts);
                    texturedDrawable->fillCommandBuffer(commandBuffer);
                    if (texturedDrawable->isHighlighted())
                    {
                        outlinePushConsts.color = texturedDrawable->getHighlightColor();
                        outlinePushConsts.lineWidth = texturedDrawable->getHighlightWidth();
                        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                outlinePushConsts);
                        texturedDrawable->fillCommandBuffer(commandBuffer);
                    }
                    m_cleanOutlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                 outlinePushConsts);
                    texturedDrawable->fillCommandBuffer(commandBuffer);
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
                    m_pipeline->bind(commandBuffer, m_transformationDescriptorSets[j][i], m_lightsDescriptorSets[0][i],
                                     m_materialDescriptorSets[j][0][i], m_textureDescriptorSets[0][i], pushConsts);
                    coloredDrawable->fillCommandBuffer(commandBuffer);
                    if (coloredDrawable->isHighlighted())
                    {
                        outlinePushConsts.color = coloredDrawable->getHighlightColor();
                        outlinePushConsts.lineWidth = coloredDrawable->getHighlightWidth();
                        m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                outlinePushConsts);
                        coloredDrawable->fillCommandBuffer(commandBuffer);
                    }
                    m_cleanOutlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                 outlinePushConsts);
                    coloredDrawable->fillCommandBuffer(commandBuffer);
                }
                else if (drawable->getDrawableType() == VulkanDrawableType::Model)
                {
                    VulkanDrawableModel *modelDrawable = dynamic_cast<VulkanDrawableModel *>(m_drawables[j].get());
                    pushConsts.calcLight = true;
                    for (size_t k = 0; k < modelDrawable->getModel().getMeshes().size(); k++)
                    {
                        VulkanMesh *mesh =
                            reinterpret_cast<VulkanMesh *>(modelDrawable->getModel().getMeshes()[k].get());
                        m_pipeline->bind(
                            commandBuffer, m_transformationDescriptorSets[j][i], m_lightsDescriptorSets[0][i],
                            m_materialDescriptorSets[j][k][i],
                            m_textureDescriptorSets[m_textureDescriptorOffset[mesh->getTexturesID() +
                                                                              mesh->getSpecularTexturesID()]][i],
                            pushConsts);
                        mesh->fillCommandBuffer(commandBuffer);
                        if (modelDrawable->isHighlighted())
                        {
                            outlinePushConsts.color = modelDrawable->getHighlightColor();
                            outlinePushConsts.lineWidth = modelDrawable->getHighlightWidth();
                            m_outlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                    outlinePushConsts);
                            mesh->fillCommandBuffer(commandBuffer);
                        }
                        m_cleanOutlinePipeline->bind(commandBuffer, m_outlineTransformationDescriptorSets[j][i],
                                                     outlinePushConsts);
                        mesh->fillCommandBuffer(commandBuffer);
                    }
                }
                else
                {
                    SPARK_CORE_ERROR("Drawable type not supported");
                }
            }
        }

        if (m_wireframe == WireframeState::Both || m_wireframe == WireframeState::Only)
        {
            struct Vulkan3DWireframePushConsts wireframePushConsts = {};
            wireframePushConsts.color = m_wireframeColor;
            for (size_t j = 0; j < m_drawables.size(); j++)
            {
                VulkanDrawableModel *drawable = dynamic_cast<VulkanDrawableModel *>(m_drawables[j].get());
                for (auto &mesh : drawable->getModel().getMeshes())
                {
                    VulkanMesh *vulkanMesh = reinterpret_cast<VulkanMesh *>(mesh.get());
                    m_wireframePipeline->bind(commandBuffer, m_transformationDescriptorSets[j][i], wireframePushConsts);
                    vulkanMesh->fillCommandBuffer(commandBuffer);
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
} // namespace Spark::Render
