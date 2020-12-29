#include "layer2d.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/layers/layer_renderer_2d.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Layer2D::Layer2D(const std::string &name)
    : Layer(name)
{
#ifdef SPARK_PLATFORM_VULKAN
    Render::VulkanRenderer &renderer = reinterpret_cast<Render::VulkanRenderer &>(Application::GetApp().GetRenderer());
    m_layer_renderer = std::make_unique<Render::VulkanLayerRenderer2D>(renderer);
#endif // SPARK_PLATFORM_VULKAN
}

void Layer2D::OnAttach()
{
    m_layer_renderer->OnAttach();
}

void Layer2D::OnDetach()
{
    m_layer_renderer->OnDetach();
}

void Layer2D::OnRender()
{
    m_layer_renderer->OnRender();
}

void Layer2D::addDrawable(std::shared_ptr<Render::Drawable> &drawable)
{
    m_layer_renderer->addDrawable(drawable);
}

void Layer2D::removeDrawable(Render::Drawable *drawable)
{
    m_layer_renderer->removeDrawable(drawable);
}
} // namespace Spark
