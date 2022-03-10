#include "model.h"

#include "spark/core/log.h"

#include "resource_manager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Spark
{
Model::Model(const std::string &name, const std::string &path)
    : Resource(name, ResourceType::model)
    , m_meshes()
    , m_directory()
    , m_overrideShininess(false)
{
    loadModel(path);
}

const std::vector<std::unique_ptr<Mesh>> &Model::getMeshes() const
{
    return m_meshes;
}

Physics::BoxBounding Model::getBounds() const
{
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto &mesh : m_meshes)
    {
        for (const auto &vertex : mesh->getVertices())
        {
            minX = std::min(vertex.pos.x, minX);
            minY = std::min(vertex.pos.y, minY);
            minZ = std::min(vertex.pos.z, minZ);
            maxX = std::max(vertex.pos.x, maxX);
            maxY = std::max(vertex.pos.y, maxY);
            maxZ = std::max(vertex.pos.z, maxZ);
        }
    }

    return Physics::BoxBounding(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)),
                                glm::scale(glm::mat4(1), glm::vec3(maxX - minX, maxY - minY, maxZ - minZ)),
                                glm::rotate(glm::mat4(1), 0.0f, glm::vec3(1, 0, 0)));
}

bool Model::overrideShininess() const
{
    return m_overrideShininess;
}

void Model::overrideShininess(bool overrideShininess)
{
    m_overrideShininess = overrideShininess;
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                       aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals);
    // check for errors
    SPARK_CORE_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode,
                      std::string("ASSIMP: ") + importer.GetErrorString())
    // retrieve the directory path of the filepath
    m_directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, aiMatrix4x4());
}

void Model::processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 transform)
{
    aiMatrix4x4 currTransform = transform * node->mTransformation;
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, currTransform);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, currTransform);
    }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 transform)
{
    // data to fill
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex3D vertex;
        aiVector3D vertexPos = transform * mesh->mVertices[i];
        vertex.pos.x = vertexPos.x;
        vertex.pos.y = vertexPos.y;
        vertex.pos.z = vertexPos.z;
        // normals
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        // texture coordinates
        // SPARK_CORE_ASSERT(mesh->GetNumUVChannels() == 1, "Only supporting models with one UV channel")
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.texCoord = {0, 0};
        }

        // tangent
        // vertex.tangent.x = mesh->mTangents[i].x;
        // vertex.tangent.y = mesh->mTangents[i].y;
        // vertex.tangent.z = mesh->mTangents[i].z;
        // // bitangent
        // vertex.bitangent.x = mesh->mBitangents[i].x;
        // vertex.bitangent.y = mesh->mBitangents[i].y;
        // vertex.bitangent.z = mesh->mBitangents[i].z;
        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding
    // vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    struct MeshBaseColor baseColor;
    baseColor.diffuse = glm::vec3(0, 0, 0);
    baseColor.specular = glm::vec3(0, 0, 0);
    baseColor.ambient = glm::vec3(0, 0, 0);
    aiColor3D color(0, 0, 0);
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        baseColor.diffuse = {color.r, color.g, color.b};
    };
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        baseColor.specular = {color.r, color.g, color.b};
    }
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        baseColor.ambient = {color.r, color.g, color.b};
    }
    float shininess = 0;
    float shininessStength = 0;
    material->Get(AI_MATKEY_SHININESS, shininess);
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStength) == AI_SUCCESS)
    {
        shininess *= shininessStength;
    }

    // 1. diffuse maps
    std::vector<const Texture *> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    // 2. specular maps
    std::vector<const Texture *> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
    // // 3. normal maps
    // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // // 4. height maps
    // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    m_meshes.push_back(createMesh(vertices, indices, baseColor, shininess, diffuseMaps, specularMaps));
}

std::vector<const Texture *> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    std::vector<const Texture *> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        if (ResourceManager::getResource(std::string("SPARK_CORE:VulkanModelImage:") + path.C_Str()) == nullptr)
        {
            textures.push_back(&ResourceManager::loadTexture(std::string("SPARK_CORE:VulkanModelImage:") + path.C_Str(),
                                                             m_directory + "/" + path.C_Str()));
        }
        else
        {
            textures.push_back(ResourceManager::getTexture(std::string("SPARK_CORE:VulkanModelImage:") + path.C_Str()));
        }
    }
    return textures;
}
} // namespace Spark