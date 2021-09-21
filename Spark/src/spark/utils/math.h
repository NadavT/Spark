#ifndef SPARK_MATH_H
#define SPARK_MATH_H

#include <glm/glm.hpp>

#include <algorithm>
#include <vector>

namespace Spark
{
static inline glm::vec3 mid_range(std::vector<glm::vec3> vectors)
{
    std::vector<float> x(vectors.size(), 0), y(vectors.size(), 0), z(vectors.size(), 0);
    std::transform(vectors.begin(), vectors.end(), x.begin(), [](glm::vec3 &v) { return v.x; });
    std::transform(vectors.begin(), vectors.end(), y.begin(), [](glm::vec3 &v) { return v.y; });
    std::transform(vectors.begin(), vectors.end(), z.begin(), [](glm::vec3 &v) { return v.z; });
    auto [x_min, x_max] = std::minmax_element(x.begin(), x.end());
    auto [y_min, y_max] = std::minmax_element(y.begin(), y.end());
    auto [z_min, z_max] = std::minmax_element(z.begin(), z.end());
    return {(*x_min + *x_max) / 2.0f, (*y_min + *y_max) / 2.0f, (*z_min + *z_max) / 2.0f};
}
} // namespace Spark

#endif /* SPARK_MATH_H */
