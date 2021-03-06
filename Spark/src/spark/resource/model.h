#ifndef SPARK_RESOURCE_MODEL_H
#define SPARK_RESOURCE_MODEL_H

#include "spark/core/core.h"
#include "spark/physics/bounding/box_bounding.h"

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

    SPARK_API virtual Physics::BoxBounding getBounds() const;

    SPARK_API virtual bool overrideShininess() const;
    SPARK_API virtual void overrideShininess(bool overrideShininess);

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
    bool m_overrideShininess;
};
} // namespace Spark

#endif /* SPARK_RESOURCE_MODEL_H */
