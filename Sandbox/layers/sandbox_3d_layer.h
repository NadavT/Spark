#ifndef SANDBOX_3D_LAYER_H
#define SANDBOX_3D_LAYER_H

#include <spark.h>

#include "editor/editor_3d_layer.h"

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
    void run();
    void pause();

    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);

    void generateBoxAdder();
    void generateObjectSetter();
    void generateDirLightSetter();
    void generateSpotLightSetter();
    void generatePointLightAdder();
    void generatePointLightSetter();
    void generateWireframeSetter();
    void generateHighlightSetter();
    void generatePointLightsSelector();

    void addEditor(Spark::Object3D &object);
    void removeEditor();
    bool deselectObject();

  private:
    Spark::Application &m_app;
    Editor3DLayer m_editorLayer;
    std::vector<std::shared_ptr<Spark::Object3D>> m_objects;
    std::vector<std::shared_ptr<Spark::Render::PointLight>> m_pointLights;
    Spark::Render::Camera m_camera;
    bool m_inEditor;
    int m_lightType;
    bool m_pickerExistLastFrame;
    int m_removeBoxIndex;
    int m_previousRemoveBoxIndex;
    int m_removePointLightIndex;
    int m_previousRemoveLightIndex;
    bool m_dirLightOn;
    bool m_spotLightOn;
    int m_wireframe;
    Spark::Object3D *m_objectToSet;
    std::array<float, 3> m_dirLightDirection;
    std::array<float, 3> m_beforeDirLightDirection;
    std::array<float, 3> m_dirLightColor;
    std::array<float, 3> m_beforeDirLightColor;
    std::array<float, 3> m_spotLightColor;
    std::array<float, 3> m_beforeSpotLightColor;
    Spark::Render::PointLight *m_pointLightToSet;
    std::array<float, 3> m_wireframeColor;
    std::array<float, 3> m_beforeWireframeColor;
};
#endif // SANDBOX_3D_LAYER_H