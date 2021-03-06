#ifndef SPARK_VULKAN_PIPELINE3D_MODEL_H
#define SPARK_VULKAN_PIPELINE3D_MODEL_H

#include "platform/vulkan/pipeline/pipeline3d.h"
#include "platform/vulkan/vertex/vertex3d.h"

namespace Spark::Render
{
struct VulkanShaderDirectionalLightModel
{
    alignas(16) glm::vec3 direction;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

#define MAX_POINT_LIGHTS (10)
struct VulkanShaderPointLightModel
{
    alignas(16) glm::vec3 position;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;

    alignas(4) float constant;
    alignas(4) float linear;
    alignas(4) float quadratic;
};

struct VulkanShaderSpotLightModel
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;

    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;

    alignas(4) float innerCutOff;
    alignas(4) float outerCutOff;
};

struct MaterialModel
{
    alignas(16) glm::vec3 baseColorDiffuse;
    alignas(16) glm::vec3 baseColorSpecular;
    alignas(16) glm::vec3 baseColorAmbient;
    alignas(4) int texturesAmount;
    alignas(4) int specularAmount;
    alignas(4) float shininess;
    alignas(16) glm::vec3 pureColor;
};

struct Vulkan3DModelConsts
{
    alignas(4) int numberOfPointLights;
    alignas(4) bool calcLight;
};

class VulkanPipeline3DModel : public VulkanPipeline
{
  public:
    VulkanPipeline3DModel(VulkanContext &context, VulkanFramebuffer &framebuffer, bool xRayHighlight = true);
    virtual ~VulkanPipeline3DModel();

    virtual void cleanup();
    virtual void recreate();

    virtual void bind(VkCommandBuffer commandBuffer, VkDescriptorSet transformationSet, VkDescriptorSet lightSet,
                      VkDescriptorSet materialSet, VkDescriptorSet textureSet, struct Vulkan3DModelConsts pushConsts);

    VkDescriptorSetLayout getMVPDescriptorSetLayout();
    VkDescriptorSetLayout getLightsDescriptorSetLayout();
    VkDescriptorSetLayout getMaterialDescriptorSetLayout();
    VkDescriptorSetLayout getTextureDescriptorSetLayout();

    void createTransformationDescriptorSets(unsigned int drawablesAmount,
                                            std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                            std::vector<std::vector<VkBuffer>> transformationUniforms);
    std::vector<VkDescriptorSet> createTransformationDescriptorSet(std::vector<VkBuffer> transformationUniforms);
    void createLightDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                   std::vector<VkBuffer> &dirLightUniform, std::vector<VkBuffer> &pointLightsUniform,
                                   std::vector<VkBuffer> &spotLightUniform);
    void createMaterialDescriptorSets(unsigned int drawablesAmount,
                                      std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                      std::vector<std::vector<VkBuffer>> materialUniform);
    void createTextureDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &texturesSets,
                                     std::vector<std::vector<VkImageView>> &textureImageViews,
                                     std::vector<std::vector<VkSampler>> &textureSamplers,
                                     std::vector<std::vector<VkImageView>> &specularImageViews,
                                     std::vector<std::vector<VkSampler>> &specularSamplers);

    void addTransformationDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &transformationSets,
                                         std::vector<std::vector<VkBuffer>> transformationUniforms,
                                         unsigned int amount = 1);
    void addMaterialDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &materialSets,
                                   std::vector<std::vector<VkBuffer>> materialUniforms, unsigned int amount = 1);
    std::vector<VkDescriptorSet> createMaterialDescriptorSet(std::vector<VkBuffer> materialUniform);
    void addTextureDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &textureSets,
                                  std::vector<std::vector<VkImageView>> textureImageView,
                                  std::vector<std::vector<VkSampler>> textureSampler,
                                  std::vector<std::vector<VkImageView>> specularImageViews,
                                  std::vector<std::vector<VkSampler>> specularSamplers, unsigned int amount = 1);

    void setXrayHighlight(bool xRay);

  private:
    using VulkanPipeline::bind;
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void updateLightsDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &lightSets,
                                    std::vector<VkBuffer> &dirLightUniform, std::vector<VkBuffer> &pointLightsUniform,
                                    std::vector<VkBuffer> &spotLightUniform);
    void updateTextureDescriptorSets(std::vector<std::vector<VkDescriptorSet>> &sets,
                                     std::vector<std::vector<VkImageView>> &textureImageViews,
                                     std::vector<std::vector<VkSampler>> &textureSamplers,
                                     std::vector<std::vector<VkImageView>> &specularImageViews,
                                     std::vector<std::vector<VkSampler>> &specularSamplers, unsigned int offset = 0);

  private:
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSetLayout m_transformationDescriptorSetLayout;
    VkDescriptorSetLayout m_lightsDescriptorSetLayout;
    VkDescriptorSetLayout m_materialDescriptorSetLayout;
    VkDescriptorSetLayout m_textureDescriptorSetLayout;
    bool m_xRayHighlight;
};
} // namespace Spark::Render

#endif /* SPARK_VULKAN_PIPELINE3D_MODEL_H */
