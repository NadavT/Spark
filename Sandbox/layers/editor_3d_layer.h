#ifndef SANDBOX_EDITOR_3D_LAYER_H
#define SANDBOX_EDITOR_3D_LAYER_H

#include <spark.h>

class Editor3DLayer : public Spark::Layer3D
{
    enum class Axis : int
    {
        X = 0,
        Y,
        Z,
        CUSTOM
    };
    enum class Transform : int
    {
        Move,
        Rotate
    };

    enum ShownTransformMap : uint8_t
    {
        Move = 0b001,
        Rotate = 0b010
    };

  public:
    explicit Editor3DLayer(Spark::Render::Camera &camera);

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Spark::Time &diffTime) override;
    void OnEvent(Spark::Event &e) override;

    void setObjectToEdit(Spark::Object3D &object);

  private:
    std::shared_ptr<Spark::Object3D> createArrow(glm::vec3 color);
    std::shared_ptr<Spark::Object3D> createRing(glm::vec3 color);

    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);
    bool handleMouseReleased(Spark::MouseButtonReleasedEvent &e);

    void updateEditorObjects();

    void findSelectedObject();
    float findSelectedMove(float currentClosest);
    float findSelectedRotate(float currentClosest);
    void initializeMoveTransform();
    void initializeRotateTransform();

    void handleMoveTransformUpdate();
    void handleRotationTransformUpdate();

    void removeAllTransforms();
    void addMoveTransforms();
    void addRotateTransforms();

  private:
    Spark::Render::Camera &m_camera;
    Spark::Object3D *m_objectToEdit;

    std::shared_ptr<Spark::Object3D> m_xArrow;
    std::shared_ptr<Spark::Object3D> m_yArrow;
    std::shared_ptr<Spark::Object3D> m_zArrow;
    std::shared_ptr<Spark::Object3D> m_xRing;
    std::shared_ptr<Spark::Object3D> m_yRing;
    std::shared_ptr<Spark::Object3D> m_zRing;
    std::shared_ptr<Spark::Object3D> m_viewRing;
    Spark::Object3D *m_selectedObject;
    Transform m_selectedTransform;
    Axis m_selectedAxis;
    float m_originalMoveAxisPosition;
    float m_originalRotatation;
    uint8_t m_shownTransforms;
};

#endif // SANDBOX_EDITOR_3D_LAYER_H
