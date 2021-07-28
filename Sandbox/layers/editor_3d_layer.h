#pragma once

#include <spark.h>

class Editor3DLayer : public Spark::Layer3D
{
  public:
    Editor3DLayer(Spark::Render::Camera &camera);

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate(Spark::Time &diffTime);
    virtual void OnEvent(Spark::Event &e);

    void setObjectPosition(glm::vec3 pos);

  private:
    std::shared_ptr<Spark::Object3D> createArrow(glm::vec3 color);

    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);

  private:
    Spark::Render::Camera &m_camera;

    std::shared_ptr<Spark::Object3D> m_xArrow;
    std::shared_ptr<Spark::Object3D> m_yArrow;
    std::shared_ptr<Spark::Object3D> m_zArrow;
};