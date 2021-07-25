#pragma once

#include "spark/core/core.h"

#include "resource.h"

#include <memory>

#define BLANK_TEXTURE_NAME "SPARK_CORE_BLANK_TEXTURE"

namespace Spark
{
class Texture : public Resource
{
  public:
    explicit Texture(const std::string &name);
    virtual ~Texture() = default;
};

SPARK_API std::unique_ptr<Texture> createTexture(const std::string &name, const std::string &path);
} // namespace Spark
