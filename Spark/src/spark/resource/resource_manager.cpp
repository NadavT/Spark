#include "resource_manager.h"

#include "spark/core/log.h"

static const char *blank_texture_path = "textures/blank.png";

namespace Spark
{
std::unordered_map<std::string, std::unique_ptr<Resource>> ResourceManager::s_resources;

void ResourceManager::Init()
{
#ifdef LOAD_BLANK_TEXTURE
    loadTexture(BLANK_TEXTURE_NAME, blank_texture_path);
#endif
}

void ResourceManager::Cleanup()
{
    s_resources.clear();
}

void ResourceManager::loadResource(std::unique_ptr<Resource> resource)
{
    s_resources[resource->getName()] = std::move(resource);
}

void ResourceManager::unloadResource(const std::string name)
{
    if (s_resources.find(name) != s_resources.end())
    {
        s_resources.erase(name);
    }
}

SPARK_API Resource *ResourceManager::getResource(const std::string &name)
{
    if (s_resources.find(name) != s_resources.end())
    {
        return s_resources[name].get();
    }

    return nullptr;
}

const Texture &ResourceManager::loadTexture(const std::string &name, const std::string &path)
{
    s_resources[name] = createTexture(name, path);
    return *reinterpret_cast<Texture *>(s_resources[name].get());
}

const Texture *ResourceManager::getTexture(const std::string &name)
{
    Resource *resource = getResource(name);
    if (resource == nullptr || resource->getType() != ResourceType::texture)
    {
        SPARK_CORE_WARN("Couldn't find requested texture: " + name);
        return nullptr;
    }

    return reinterpret_cast<Texture *>(resource);
}

SPARK_API const Model &ResourceManager::loadModel(const std::string &name, const std::string &path,
                                                  bool overrideShininess)
{
    s_resources[name] = std::make_unique<Model>(name, path);
    reinterpret_cast<Model *>(s_resources[name].get())->overrideShininess(true);
    return *reinterpret_cast<Model *>(s_resources[name].get());
}

SPARK_API const Model *ResourceManager::getModel(const std::string &name)
{
    Resource *resource = getResource(name);
    if (resource == nullptr || resource->getType() != ResourceType::model)
    {
        SPARK_CORE_WARN("Couldn't find requested texture: " + name);
        return nullptr;
    }

    return reinterpret_cast<Model *>(resource);
}
} // namespace Spark
