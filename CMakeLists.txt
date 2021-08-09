cmake_minimum_required(VERSION 3.7.2)
project(spark_app)

set(APP_EXECUTABLE_PATH ${PROJECT_BINARY_DIR}/Sandbox)

add_subdirectory(Spark)
add_subdirectory(Sandbox)
