#ifndef SANDBOX_EDITOR_3D_SCALE_H
#define SANDBOX_EDITOR_3D_SCALE_H

#include <spark.h>

class Editor3DScale
{
    enum class Axis : int
    {
        X = 0,
        Y,
        Z,
        View
    };

  public:
    explicit Editor3DScale(Spark::Layer3D &layer);
    float findSelectedScale(Spark::Render::Camera &camera);
    void initializeScaleTransform(Spark::Render::Camera &camera);
    void handleScaleTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit);

    void addTransforms();
    void removeTransforms();

    void release();

  private:
    Spark::Layer3D &m_layer;
    std::shared_ptr<Spark::Object3D> m_xArrow;
    std::shared_ptr<Spark::Object3D> m_yArrow;
    std::shared_ptr<Spark::Object3D> m_zArrow;
    std::shared_ptr<Spark::Object3D> m_viewRing;
    Spark::Object3D *m_selectedObject;
    Axis m_selectedAxis;
    float m_originalScaleAxisPosition;
};

#endif /* SANDBOX_EDITOR_3D_SCALE_H */
