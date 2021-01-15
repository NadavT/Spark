#pragma once
#include <array>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace Spark
{
struct Vertex3D
{
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const Vertex3D &other) const
    {
        return pos == other.pos && texCoord == other.texCoord && normal == other.normal;
    }
};
} // namespace Spark

template <> struct std::hash<Spark::Vertex3D>
{
    size_t operator()(Spark::Vertex3D const &vertex) const
    {
        return (std::hash<glm::vec3>()(vertex.pos)) ^ (std::hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
