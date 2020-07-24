#pragma once

#include "spark/core/core.h"

#include <unordered_map>
#include <memory>

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

        SPARK_API static Resource* getResource(const std::string& name);

        SPARK_API static const Texture& loadTexture(const std::string& name, const std::string& path);
        SPARK_API static const Texture* getTexture(const std::string& name);

    private:
        ResourceManager() = delete;
        void operator=(ResourceManager const&)  = delete;

    private:
        static std::unordered_map<std::string, std::unique_ptr<Resource>> s_resources;
    };
}
