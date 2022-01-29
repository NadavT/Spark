#ifndef SANDBOX_EDITOR_3D_MOVE_H
#define SANDBOX_EDITOR_3D_MOVE_H

#include <spark.h>

class Editor3DMove
{
    enum class Axis : int
    {
        X = 0,
        Y,
        Z,
    };

  public:
    explicit Editor3DMove(Spark::Layer3D &layer);
    float findSelectedMove(Spark::Render::Camera &camera);
    void initializeMoveTransform(Spark::Render::Camera &camera);
    void handleMoveTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void addTransforms(bool minimal = false);
    void removeTransforms();

    void release();

  private:
    Spark::Layer3D &m_layer;
    std::shared_ptr<Spark::Object3D> m_xArrow;
    std::shared_ptr<Spark::Object3D> m_yArrow;
    std::shared_ptr<Spark::Object3D> m_zArrow;
    Spark::Object3D *m_selectedObject;
    Axis m_selectedAxis;
    float m_originalMoveAxisPosition;
    bool m_minimal;
    bool m_addObjects;
};

#endif /* SANDBOX_EDITOR_3D_MOVE_H */
