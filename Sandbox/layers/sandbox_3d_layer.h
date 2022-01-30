#ifndef SANDBOX_3D_LAYER_H
#define SANDBOX_3D_LAYER_H

#include <spark.h>

#include "sandbox_3d/overlay/overlay_3d.h"
#include "sandbox_3d/sandbox_3d.h"

class Sandbox3DLayer : public Spark::Layer3D
{
  public:
    Sandbox3DLayer(Spark::Application &app);

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate(Spark::Time &diffTime);
    virtual void OnEvent(Spark::Event &e);
    virtual void generateOverlay();

  private:
    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);

  private:
    Spark::Application &m_app;
    Spark::Render::Camera m_camera;
    Sandbox3D m_sandbox;
    Overlay3D m_overlay;
};
#endif // SANDBOX_3D_LAYER_H