#include "layer3d.h"

#include "spark/core/application.h"

#ifdef SPARK_PLATFORM_VULKAN
    #include "platform/vulkan/layers/layer_renderer_3d_lights.h"
    #include "platform/vulkan/layers/layer_renderer_3d_model.h"
    #include "spark/renderer/drawables/box.h"
#endif // SPARK_PLATFORM_VULKAN

namespace Spark
{
Layer3D::Layer3D(const std::string &name, Render::Camera &camera)
    : Layer(name)
    , m_camera(camera)
{
#ifdef SPARK_PLATFORM_VULKAN
    Render::VulkanRenderer &renderer = reinterpret_cast<Render::VulkanRenderer &>(Application::GetApp().GetRenderer());
    m_layer_renderer = std::make_unique<Render::VulkanLayerRenderer3DModel>(renderer, m_camera);
#endif // SPARK_PLATFORM_VULKAN
}

void Layer3D::OnAttach()
{
    Layer::OnAttach();
    m_layer_renderer->OnAttach();
}

void Layer3D::OnDetach()
{
    Layer::OnDetach();
    m_layer_renderer->OnDetach();
}

void Layer3D::OnRender()
{
    Layer::OnRender();
    m_layer_renderer->OnRender();
}

void Layer3D::addDrawable(std::shared_ptr<Render::Drawable> &drawable)
{
    m_layer_renderer->addDrawable(drawable);
}

void Layer3D::addObject(Object3D &object)
{
    addDrawable(std::dynamic_pointer_cast<Render::Drawable>(object.getDrawable()));
}

void Layer3D::removeDrawable(Render::Drawable *drawable)
{
    m_layer_renderer->removeDrawable(drawable);
}

void Layer3D::removeObject(Object3D &object)
{
    object.removeParent();
    for (auto &child : object.getChilds())
    {
        child->removeParent();
    }

    removeDrawable(object.getDrawable().get());
}

void Layer3D::setDirLight(glm::vec3 direction, glm::vec3 color)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())->setDirLight(direction, color);
#endif
}

SPARK_API void Layer3D::addPointLight(Render::PointLight &pointLight)
{
#ifdef SPARK_PLATFORM_VULKAN
    addDrawable(std::dynamic_pointer_cast<Render::Drawable>(pointLight.getDrawable()));
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())
        ->addPointLight(reinterpret_cast<Render::VulkanPointLight &>(pointLight));
#endif
}

SPARK_API void Layer3D::removePointLight(Render::PointLight &pointLight)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())
        ->removePointLight(reinterpret_cast<Render::VulkanPointLight &>(pointLight));
    m_layer_renderer->removeDrawable(pointLight.getDrawable().get());
#endif
}

void Layer3D::setSpotLight(glm::vec3 direction)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())->setSpotLight(direction);
#endif
}

SPARK_API void Layer3D::setWireframe(Render::WireframeState state, glm::vec3 color)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())->setWireframe(state, color);
#endif
}

SPARK_API bool Layer3D::getXrayHighlight() const
{
#ifdef SPARK_PLATFORM_VULKAN
    return reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())->getXrayHighlight();
#endif
}

SPARK_API void Layer3D::setXrayHighlight(bool xRayHighlight)
{
#ifdef SPARK_PLATFORM_VULKAN
    reinterpret_cast<Render::VulkanLayerRenderer3DModel *>(m_layer_renderer.get())->setXrayHighlight(xRayHighlight);
#endif
}
} // namespace Spark
