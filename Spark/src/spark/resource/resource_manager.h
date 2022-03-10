#ifndef SPARK_RESOURCE_MANAGER_H
#define SPARK_RESOURCE_MANAGER_H

#include "spark/core/core.h"

#include <memory>
#include <unordered_map>

#include "model.h"
#include "texture.h"

namespace Spark
{
class ResourceManager
{
  public:
    static void Init();
    static void Cleanup();

    SPARK_API static void loadResource(std::unique_ptr<Resource> resource);
    SPARK_API static void unloadResource(const std::string name);

    SPARK_API static Resource *getResource(const std::string &name);

    SPARK_API static const Texture &loadTexture(const std::string &name, const std::string &path);
    SPARK_API static const Texture *getTexture(const std::string &name);

    SPARK_API static const Model &loadModel(const std::string &name, const std::string &path,
                                            bool overrideShininess = false);
    SPARK_API static const Model *getModel(const std::string &name);

  private:
    ResourceManager() = delete;
    void operator=(ResourceManager const &) = delete;

  private:
    static std::unordered_map<std::string, std::unique_ptr<Resource>> s_resources;
};
} // namespace Spark

#endif /* SPARK_RESOURCE_MANAGER_H */
