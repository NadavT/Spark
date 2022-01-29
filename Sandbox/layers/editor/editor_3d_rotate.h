#ifndef SANDBOX_EDITOR_3D_ROTATE_H
#define SANDBOX_EDITOR_3D_ROTATE_H

#include <spark.h>

class Editor3DRotate
{
    enum class Axis : int
    {
        X = 0,
        Y,
        Z,
        VIEW
    };

  public:
    explicit Editor3DRotate(Spark::Layer3D &layer);
    float findSelectedRotate(Spark::Render::Camera &camera);
    void initializeRotateTransform(Spark::Render::Camera &camera);
    void handleRotateTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void addTransforms();
    void removeTransforms();

    void release();

  private:
    Spark::Layer3D &m_layer;
    std::shared_ptr<Spark::Object3D> m_xRing;
    std::shared_ptr<Spark::Object3D> m_yRing;
    std::shared_ptr<Spark::Object3D> m_zRing;
    std::shared_ptr<Spark::Object3D> m_viewRing;
    Spark::Object3D *m_selectedObject;
    Axis m_selectedAxis;
    float m_originalRotation;
};

#endif /* SANDBOX_EDITOR_3D_ROTATE_H */
