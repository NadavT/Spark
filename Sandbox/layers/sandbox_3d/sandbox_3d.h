#ifndef SPARK_SANDBOX_3D_H
#define SPARK_SANDBOX_3D_H

#include <spark.h>

#include "editor/editor_3d_layer.h"

class Sandbox3D
{
  public:
    enum class PointLightType
    {
        Sphere,
        Cube
    };

  public:
    Sandbox3D(Spark::Application &app, Spark::Layer3D &layer, Spark::Render::Camera &camera);
    virtual ~Sandbox3D() = default;

    virtual void OnUpdate(Spark::Time &diffTime);
    virtual void OnEvent(Spark::Event &e);

    bool isInEditor() const;
    void addBox();
    void addPointLight(PointLightType type, const glm::vec3 &position = {0, 0, 0}, const glm::vec3 &color = {1, 1, 1});
    void removePointLight(Spark::Render::PointLight *pointLight);
    Spark::Object3D *getObjectToSet();
    Spark::Render::PointLight *getPointLightToSet();
    const std::vector<std::shared_ptr<Spark::Render::PointLight>> &getPointLights();

    void run();
    void pause();

    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleKeyReleased(Spark::KeyReleasedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);
    bool handleMouseReleased(Spark::MouseButtonReleasedEvent &e);
    bool handleMouseScroll(Spark::MouseScrolledEvent &e);

    void addEditor(Spark::Object3D &object);
    void removeEditor();
    void selectObject(Spark::Object3D *object);
    void selectPointLight(Spark::Render::PointLight *pointLight);
    bool deselectObject();

  private:
    bool escape();
    bool toggleRun();
    bool removeSelectedObject();

    bool pickObject();

  private:
    Spark::Application &m_app;
    Spark::Layer3D &m_layer;
    Editor3DLayer m_editorLayer;
    std::vector<std::shared_ptr<Spark::Object3D>> m_objects;
    std::vector<std::shared_ptr<Spark::Render::PointLight>> m_pointLights;
    Spark::Render::Camera &m_camera;
    bool m_inEditor;
    Spark::Object3D *m_objectToSet;
    Spark::Render::PointLight *m_pointLightToSet;

    bool m_panning;
    bool m_rotating;
};

#endif /* SPARK_SANDBOX_3D_H */
