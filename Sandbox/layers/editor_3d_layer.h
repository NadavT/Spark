#pragma once

#include <spark.h>

enum class Axis
{
    X = 0,
    Y,
    Z
};
class Editor3DLayer : public Spark::Layer3D
{
  public:
    explicit Editor3DLayer(Spark::Render::Camera &camera);

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Spark::Time &diffTime) override;
    void OnEvent(Spark::Event &e) override;

    void setObjectToEdit(Spark::Object3D &object);

  private:
    std::shared_ptr<Spark::Object3D> createArrow(glm::vec3 color);

    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);
    bool handleMouseReleased(Spark::MouseButtonReleasedEvent &e);

  private:
    Spark::Render::Camera &m_camera;
    Spark::Object3D *m_objectToEdit;

    std::shared_ptr<Spark::Object3D> m_xArrow;
    std::shared_ptr<Spark::Object3D> m_highlightedXArrow;
    std::shared_ptr<Spark::Object3D> m_yArrow;
    std::shared_ptr<Spark::Object3D> m_highlightedYArrow;
    std::shared_ptr<Spark::Object3D> m_zArrow;
    std::shared_ptr<Spark::Object3D> m_highlightedZArrow;
    Spark::Object3D *m_selectedArrow;
    Axis m_selectedAxis;
    float m_originalAxisPosition;
};