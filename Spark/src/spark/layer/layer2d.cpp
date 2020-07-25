#include "layer2d.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
#include "platform/vulkan/layers/layer_renderer_2d.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
    Layer2D::Layer2D(const std::string& name)
        : Layer(name)
    {
#ifdef SPARK_PLATFORM_VULKAN
        VulkanRenderer& renderer = reinterpret_cast<VulkanRenderer&>(Application::GetApp().GetRenderer());
        m_layer_renderer = std::make_unique<VulkanLayerRenderer2D>(renderer);
#endif // SPARK_PLATFORM_VULKAN
    }
    
    SPARK_API void Layer2D::OnAttach() 
    {
        m_layer_renderer->OnAttach();
    }
    
    SPARK_API void Layer2D::OnDetach() 
    {
        m_layer_renderer->OnDetach();
    }
    
    SPARK_API void Layer2D::OnRender() 
    {
        m_layer_renderer->OnRender();
    }
    
    SPARK_API void Layer2D::addDrawable(Drawable* drawable) 
    {
        m_layer_renderer->addDrawable(drawable);
    }
    
    SPARK_API void Layer2D::removeDrawable(Drawable* drawable) 
    {
        m_layer_renderer->removeDrawable(drawable);
    }
}
