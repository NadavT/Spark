#pragma once

#include "spark/vertex/vertex3d.h"
#include "texture.h"

#include <array>
#include <string>
#include <vector>

namespace Spark
{
struct MeshBaseColor
{
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
};
class Mesh
{
  public:
    virtual ~Mesh() = default;

    virtual const std::vector<const Texture *> &getTextures() const;
    virtual const std::vector<const Texture *> &getSpecularTextures() const;

    virtual struct MeshBaseColor getBaseColor() const;
    virtual float getShininess() const;
    virtual std::string getTexturesID() const;
    virtual std::string getSpecularTexturesID() const;
    virtual const std::vector<Vertex3D> &getVertices() const;
    virtual const std::vector<unsigned int> &getIndices() const;

    virtual std::vector<std::array<Vertex3D, 3>> getFaces() const;

  protected:
    Mesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices, struct MeshBaseColor baseColor,
         float shininess, std::vector<const Texture *> &textures, std::vector<const Texture *> &specularTextures);
    std::vector<Vertex3D> m_vertices;
    std::vector<unsigned int> m_indices;
    struct MeshBaseColor m_baseColor;
    float m_shininess;
    std::vector<const Texture *> m_textures;
    std::vector<const Texture *> m_specularTextures;
};

std::unique_ptr<Mesh> createMesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices,
                                 struct MeshBaseColor baseColor, float shininess,
                                 std::vector<const Texture *> &textures,
                                 std::vector<const Texture *> &specularTextures);
} // namespace Spark
