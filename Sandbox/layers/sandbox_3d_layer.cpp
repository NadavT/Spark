#include "sandbox_3d_layer.h"

#include <limits>

Sandbox3DLayer::Sandbox3DLayer(Spark::Application &app)
    : m_camera({10.0f, 0.0f, 0.0f}, 90.0f)
    , Spark::Layer3D("Sandbox 3d layer", m_camera)
    , m_app(app)
    , m_sandbox(app, *this, m_camera)
    , m_overlay(*this, m_sandbox)
{
}

void Sandbox3DLayer::OnAttach()
{
    SPARK_INFO("Attached {0}", getName());
    Layer3D::OnAttach();
}

void Sandbox3DLayer::OnDetach()
{
    Spark::Input::UnHideMouse();
    Layer3D::OnDetach();
    SPARK_INFO("Detached {0}", getName());
}

void Sandbox3DLayer::OnUpdate(Spark::Time &diffTime)
{
    Layer3D::OnUpdate(diffTime);
    m_sandbox.OnUpdate(diffTime);
}

void Sandbox3DLayer::OnEvent(Spark::Event &e)
{
    Layer3D::OnEvent(e);
    m_sandbox.OnEvent(e);
}

void Sandbox3DLayer::generateOverlay()
{
    if (m_sandbox.isInEditor())
    {
        m_overlay.generateOverlay();
    }
}

bool Sandbox3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    return m_sandbox.handleMouseMoved(e);
}

bool Sandbox3DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    return m_sandbox.handleKeyPressed(e);
}

bool Sandbox3DLayer::handleMousePressed(Spark::MouseButtonPressedEvent &e)
{
    return m_sandbox.handleMousePressed(e);
}
