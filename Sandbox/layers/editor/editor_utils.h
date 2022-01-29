#ifndef SPARK_EDITOR_UTILS_H
#define SPARK_EDITOR_UTILS_H

#include <memory>
#include <vector>

#include <spark.h>

enum class ArrowHead
{
    Cone,
    Cube
};

std::shared_ptr<Spark::Object3D> createArrowHead(glm::vec3 color, ArrowHead type, float length = 2.0f,
                                                 float headSize = 0.5f);
std::shared_ptr<Spark::Object3D> createArrow(glm::vec3 color, ArrowHead type, float length = 2.0f,
                                             float headSize = 0.5f, float bodyWidth = 0.2f);
std::shared_ptr<Spark::Object3D> createRing(glm::vec3 color);

#endif /* SPARK_EDITOR_UTILS_H */
