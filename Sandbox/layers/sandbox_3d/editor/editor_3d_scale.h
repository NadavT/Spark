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

    void addTransforms(bool minimal = false);
    void removeTransforms();

    void release();

  private:
    Spark::Layer3D &m_layer;
    std::shared_ptr<Spark::Object3D> m_xArrowObject;
    std::shared_ptr<Spark::Object3D> m_yArrowObject;
    std::shared_ptr<Spark::Object3D> m_zArrowObject;
    std::shared_ptr<Spark::Object3D> m_viewRingObject;
    std::shared_ptr<Spark::Object3D> m_minimalXArrowObject;
    std::shared_ptr<Spark::Object3D> m_minimalYArrowObject;
    std::shared_ptr<Spark::Object3D> m_minimalZArrowObject;
    std::shared_ptr<Spark::Object3D> m_minimalViewRingObject;

    Spark::Object3D *m_xArrow;
    Spark::Object3D *m_yArrow;
    Spark::Object3D *m_zArrow;
    Spark::Object3D *m_viewRing;
    Spark::Object3D *m_selectedObject;
    Axis m_selectedAxis;
    float m_originalScaleAxisPosition;
    bool m_minimal;
    bool m_addObjects;
};

#endif /* SANDBOX_EDITOR_3D_SCALE_H */
