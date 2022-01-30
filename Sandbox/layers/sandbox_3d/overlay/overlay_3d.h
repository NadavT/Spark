#ifndef SPARK_OVERLAY_3D_H
#define SPARK_OVERLAY_3D_H

#include <spark.h>

#include "layers/sandbox_3d/sandbox_3d.h"

class Overlay3D
{
  public:
    Overlay3D(Spark::Layer3D &layer, Sandbox3D &sandbox);
    virtual ~Overlay3D() = default;

    void generateOverlay();

  private:
    void generateBoxAdder();
    void generateObjectSetter();
    void generateDirLightSetter();
    void generateSpotLightSetter();
    void generatePointLightAdder();
    void generatePointLightSetter();
    void generateWireframeSetter();
    void generateHighlightSetter();
    void generatePointLightsSelector();

  private:
    Spark::Layer3D &m_layer;
    Sandbox3D &m_sandbox;
    int m_lightType;
    bool m_dirLightOn;
    bool m_spotLightOn;
    int m_wireframe;
    std::array<float, 3> m_dirLightDirection;
    std::array<float, 3> m_beforeDirLightDirection;
    std::array<float, 3> m_dirLightColor;
    std::array<float, 3> m_beforeDirLightColor;
    std::array<float, 3> m_spotLightColor;
    std::array<float, 3> m_beforeSpotLightColor;
    std::array<float, 3> m_wireframeColor;
    std::array<float, 3> m_beforeWireframeColor;
};

#endif /* SPARK_OVERLAY_3D_H */
