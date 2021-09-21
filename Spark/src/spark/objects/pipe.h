#ifndef SPARK_PIPE_H
#define SPARK_PIPE_H

#include "object.h"

#include "spark/resource/texture.h"

namespace Spark
{
class Pipe : public Object3D
{
  public:
    SPARK_API Pipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color, int sectors = 36);
    SPARK_API Pipe(std::vector<glm::vec3> positions, float radius, const Texture &texture,
                   const Texture &specularTexture, int sectors = 36);
    SPARK_API virtual ~Pipe() = default;
};

SPARK_API std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color,
                                               int sectors = 36);
SPARK_API std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, const Texture &texture,
                                               const Texture &specularTexture, int sectors = 36);
} // namespace Spark

#endif /* SPARK_PIPE_H */
