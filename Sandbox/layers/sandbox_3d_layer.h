#pragma once

#include <spark.h>

class Sandbox3DLayer : public Spark::Layer3D
{
  public:
    Sandbox3DLayer();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate(Spark::Time &diffTime);
    virtual void OnEvent(Spark::Event &e);
    virtual void generateOverlay();

  private:
    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);

    void generateBoxAdder();
    void generateBoxRemover();
    void generateDirLightSetter();
    void generateSpotLightSetter();
    void generatePointLightAdder();
    void generatePointLightRemover();
    void generateWireframeSetter();
    void generatePointLightsFlicker();

  private:
    std::vector<std::shared_ptr<Spark::Drawable>> m_drawables;
    std::vector<std::shared_ptr<Spark::PointLight>> m_pointLights;
    Spark::Camera m_camera;
    bool m_focused;
    int m_lightType;
    int m_removeBoxIndex;
    int m_previousRemoveBoxIndex;
    int m_removePointLightIndex;
    int m_previousRemoveLightIndex;
    bool m_dirLightOn;
    bool m_spotLightOn;
    int m_wireframe;
    float m_nextCords[3];
    float m_dirLightDirection[3];
    float m_dirLightColor[3];
    float m_spotLightColor[3];
    float m_nextPointLightCords[3];
    float m_pointLightColor[3];
    float m_wireframeColor[3];
    float m_beforeWireframeColor[3];
};