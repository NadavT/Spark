#include "quad.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/renderer.h"
#include "platform/vulkan/drawables/quad.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
    Quad::Quad(glm::vec2 position) 
        : m_transformation()
    {
        m_transformation = glm::translate(glm::mat3(1.0), position);
    }
    
    void Quad::move(glm::vec2 position) 
    {
		m_transformation = glm::translate(m_transformation, position);
    }
    
    SPARK_API glm::mat3 Quad::getTransformation() 
    {
        return m_transformation;
    }

    std::unique_ptr<Quad> createQuad(glm::vec2 position, const Texture& texture) 
    {
#ifdef SPARK_PLATFORM_VULKAN
        VulkanRenderer& renderer = reinterpret_cast<VulkanRenderer&>(Application::GetApp().GetRenderer());
        const VulkanTexture& vulkanTexture = reinterpret_cast<const VulkanTexture&>(texture);
        return std::make_unique<VulkanQuad>(renderer.getContext(), position, vulkanTexture);
#endif // SPARK_PLATFORM_VULKAN
    }
}
