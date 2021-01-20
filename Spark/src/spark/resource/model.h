#pragma once

#include "spark/core/core.h"

#include "mesh.h"
#include "resource.h"
#include "texture.h"

#include <memory>
#ifdef SPARK_BUILD_DLL
    #include <assimp/scene.h>
#endif

namespace Spark
{
class Model : public Resource
{
  public:
    Model(const std::string &name, const std::string &path);
    SPARK_API virtual ~Model() = default;

    const std::vector<std::unique_ptr<Mesh>> &getMeshes() const;

  private:
#ifdef SPARK_BUILD_DLL
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 transform);
    void processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 transform);
    std::vector<const Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type);
#endif

  private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::string m_directory;
};
} // namespace Spark
