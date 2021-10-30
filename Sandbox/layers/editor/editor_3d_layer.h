#ifndef SANDBOX_EDITOR_3D_LAYER_H
#define SANDBOX_EDITOR_3D_LAYER_H

#include <spark.h>

#include "editor_3d_move.h"
#include "editor_3d_rotate.h"

class Editor3DLayer : public Spark::Layer3D
{
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
    bool handleMouseMoved(Spark::MouseMovedEvent &e);
    bool handleKeyPressed(Spark::KeyPressedEvent &e);
    bool handleMousePressed(Spark::MouseButtonPressedEvent &e);
    bool handleMouseReleased(Spark::MouseButtonReleasedEvent &e);

    void updateEditorObjects();

    void findSelectedObject();

    void removeAllTransforms();

  private:
    Spark::Render::Camera &m_camera;
    Spark::Object3D *m_objectToEdit;
    Editor3DMove m_moveTransform;
    Editor3DRotate m_rotateTransform;

    bool m_selected;
    Transform m_selectedTransform;
    uint8_t m_shownTransforms;
};

#endif // SANDBOX_EDITOR_3D_LAYER_H
