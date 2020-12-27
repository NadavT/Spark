#include "layer3d.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/layers/layer_renderer_3d_lights.h"
    #include "spark/renderer/drawables/cube.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Layer3D::Layer3D(const std::string &name, Camera &camera)
    : Layer(name)
    , m_camera(camera)
{
#ifdef SPARK_PLATFORM_VULKAN
    VulkanRenderer &renderer = reinterpret_cast<VulkanRenderer &>(Application::GetApp().GetRenderer());
    m_layer_renderer = std::make_unique<VulkanLayerRenderer3DLights>(renderer, m_camera);
#endif // SPARK_PLATFORM_VULKAN
}

void Layer3D::OnAttach()
{
    m_layer_renderer->OnAttach();
}

void Layer3D::OnDetach()
{
    m_layer_renderer->OnDetach();
}

void Layer3D::OnRender()
{
    m_layer_renderer->OnRender();
}

void Layer3D::addDrawable(std::shared_ptr<Drawable> &drawable)
{
    m_layer_renderer->addDrawable(drawable);
}

void Layer3D::removeDrawable(Drawable *drawable)
{
    m_layer_renderer->removeDrawable(drawable);
}

void Layer3D::setDirLight(glm::vec3 direction, glm::vec3 color)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<VulkanLayerRenderer3DLights *>(m_layer_renderer.get())->setDirLight(direction, color);
#endif
}

SPARK_API void Layer3D::addPointLight(PointLight &pointLight)
{
#ifdef SPARK_PLATFORM_VULKAN
    addDrawable(std::dynamic_pointer_cast<Drawable>(pointLight.drawable));
    reinterpret_cast<VulkanLayerRenderer3DLights *>(m_layer_renderer.get())
        ->addPointLight(reinterpret_cast<VulkanPointLight &>(pointLight));
#endif
}

SPARK_API void Layer3D::removePointLight(PointLight &pointLight)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<VulkanLayerRenderer3DLights *>(m_layer_renderer.get())
        ->removePointLight(reinterpret_cast<VulkanPointLight &>(pointLight));
    m_layer_renderer->removeDrawable(pointLight.drawable.get());
#endif
}

void Layer3D::setSpotLight(glm::vec3 direction)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<VulkanLayerRenderer3DLights *>(m_layer_renderer.get())->setSpotLight(direction);
#endif
}

SPARK_API void Layer3D::setWireframe(WireframeState state, glm::vec3 color)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<VulkanLayerRenderer3DLights *>(m_layer_renderer.get())->setWireframe(state, color);
#endif
}
} // namespace Spark
