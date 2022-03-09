#include "layer.h"

namespace Spark
{
Layer::Layer(const std::string &name)
    : m_name(name)
    , m_isAttached(false)
{
}

const char *Layer::getName() const
{
    return m_name.c_str();
}
} // namespace Spark
