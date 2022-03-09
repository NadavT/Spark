#ifndef SPARK_RESOURCE_RESOURCE_H
#define SPARK_RESOURCE_RESOURCE_H

#include <string>

namespace Spark
{
enum class ResourceType
{
    texture,
    model,
    internal
};

class Resource
{
  public:
    virtual ~Resource() = default;

    std::string getName() const
    {
        return m_name;
    };
    ResourceType getType() const
    {
        return m_type;
    };

  protected:
    Resource(const std::string &name, ResourceType type)
        : m_name(name)
        , m_type(type){};

  private:
    std::string m_name;
    ResourceType m_type;
};
} // namespace Spark

#endif /* SPARK_RESOURCE_RESOURCE_H */
