#pragma once

#include "platform/vulkan/pipeline/pipeline.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
struct Transformation3DLights
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct VulkanShaderDirectionalLight
{
    alignas(16) glm::vec3 direction;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

#define MAX_POINT_LIGHTS (10)
struct VulkanShaderPointLight
{
    alignas(16) glm::vec3 position;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;

    alignas(4) float constant;
    alignas(4) float linear;
    alignas(4) float quadratic;
};

struct VulkanShaderSpotLight
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;

    float innerCutOff;
    float outerCutOff;
};

struct Material
{
    float shininess;
};

struct Vulkan3DLightsConsts
{
    alignas(4) int numberOfPointLights;
    alignas(4) bool calcLight;
    alignas(16) glm::vec3 color;
};

class VulkanPipeline3DLights : public VulkanPipeline
{
  public:
    VulkanPipeline3DLights(VulkanContext &context, VulkanFramebuffer &framebuffer);
    virtual ~VulkanPipeline3DLights();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet lightSet,
                      VkDescriptorSet materialSet, VkDescriptorSet textureSet, struct Vulkan3DLightsConsts pushConsts);
    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet lightSet,
                      VkDescriptorSet materialSet, struct Vulkan3DLightsConsts pushConsts);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();
    VkDescriptorSetLayout getLightsDescriptorSetLayout();
    VkDescriptorSetLayout getMaterialDescriptorSetLayout();
    VkDescriptorSetLayout getTextureDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);
    void createLightDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                   std::vector<VkBuffer> &dirLightUniform, std::vector<VkBuffer> &pointLightsUniform,
                                   std::vector<VkBuffer> &spotLightUniform);
    void createMaterialDescriptorSets(unsigned int drawablesAmount,
                                      std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                      std::vector<std::vector<VkBuffer>> materialUniform);
    void createTextureDescriptorSets(unsigned int texturesAmount,
                                     std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers,
                                     std::vector<VkImageView> &specularImageViews,
                                     std::vector<VkSampler> &specularSamplers);

    void createSingleTransformationDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                                 std::vector<VkBuffer> transformationUniforms);
    void createSingleMaterialDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                           std::vector<VkBuffer> materialUniforms);
    void createSingleTextureDescriptorSet(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                          VkImageView textureImageView, VkSampler textureSampler,
                                          VkImageView specularImageView, VkSampler specularSampler);

  private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void updateLightsDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                    std::vector<VkBuffer> &dirLightUniform, std::vector<VkBuffer> &pointLightsUniform,
                                    std::vector<VkBuffer> &spotLightUniform);
    void updateTextureDescriptorSets(unsigned int amount, std::vector<std::vector<VkDescriptorSet>> &sets,
                                     std::vector<VkImageView> &textureImageViews,
                                     std::vector<VkSampler> &textureSamplers,
                                     std::vector<VkImageView> &specularImageViews,
                                     std::vector<VkSampler> &specularSamplers, unsigned int offset = 0);

  private:
    VkPipelineLayout m_pipelineColorLayout;
    VkPipelineLayout m_pipelineTextureLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
    VkDescriptorSetLayout m_lightsDescriptorSetLayout;
    VkDescriptorSetLayout m_materialDescriptorSetLayout;
    VkDescriptorSetLayout m_textureDescriptorSetLayout;
    VkPipeline m_pipelineColor;
    VkPipeline m_pipelineTexture;
};
} // namespace Spark::Render
