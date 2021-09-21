#ifndef SPARK_RENDER_PIPE_H
#define SPARK_RENDER_PIPE_H

#include "spark/core/core.h"

#include "spark/renderer/drawables/drawable3d.h"
#include "spark/resource/texture.h"

#include <memory>

namespace Spark::Render
{
class Pipe : public virtual Drawable3D
{
  public:
    SPARK_API virtual ~Pipe() = default;
    SPARK_API Pipe(const Pipe &other) = default;
    SPARK_API Pipe(Pipe &&other) noexcept = default;

  protected:
    SPARK_API Pipe(std::vector<glm::vec3> positions, float radius, int sectors, glm::vec3 scale = glm::vec3(1.0));

  protected:
    std::vector<glm::vec3> m_positions;
    float m_radius;
    int m_sectors;
};

SPARK_API std::unique_ptr<Drawable3D> createPipe(std::vector<glm::vec3> positions, float radius, const Texture &texture,
                                                 const Texture &specularTexture, int sectors = 36,
                                                 glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::unique_ptr<Drawable3D> createPipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color,
                                                 int sectors = 36, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark::Render

#endif /* SPARK_RENDER_PIPE_H */
