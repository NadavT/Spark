#ifndef SPARK_FILE_H
#define SPARK_FILE_H

#include <string>
#include <vector>

namespace Spark
{
std::vector<char> readFile(const std::string &filename);
}

#endif /* SPARK_FILE_H */
