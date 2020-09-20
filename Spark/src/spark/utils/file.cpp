#include "file.h"

#include "spark/core/log.h"

#include <fstream>

namespace Spark
{
std::vector<char> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        SPARK_CORE_ERROR("failed to open file: {0}\n", filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
} // namespace Spark
