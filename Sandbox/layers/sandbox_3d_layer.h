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
    std::vector<std::shared_ptr<Spark::Drawable3D>> m_drawables;
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
    std::array<float, 3> m_nextCords;
    std::array<float, 3> m_dirLightDirection;
    std::array<float, 3> m_beforeDirLightDirection;
    std::array<float, 3> m_dirLightColor;
    std::array<float, 3> m_beforeDirLightColor;
    std::array<float, 3> m_spotLightColor;
    std::array<float, 3> m_beforeSpotLightColor;
    std::array<float, 3> m_nextPointLightCords;
    std::array<float, 3> m_pointLightColor;
    std::array<float, 3> m_wireframeColor;
    std::array<float, 3> m_beforeWireframeColor;
};