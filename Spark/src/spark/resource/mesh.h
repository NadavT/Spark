#pragma once

#include "spark/vertex/vertex3d.h"
#include "texture.h"

#include <vector>

namespace Spark
{
class Mesh
{
  public:
    Mesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices, std::vector<const Texture *> &textures);
    virtual ~Mesh() = default;

  protected:
    std::vector<Vertex3D> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<const Texture *> m_textures;
};
} // namespace Spark
