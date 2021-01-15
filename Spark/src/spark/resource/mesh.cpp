#include "mesh.h"

namespace Spark
{
Mesh::Mesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices, std::vector<const Texture *> &textures)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_textures(textures)
{
}
} // namespace Spark