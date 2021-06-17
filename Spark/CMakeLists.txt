project(spark)

# Used policies
cmake_policy(SET CMP0076 NEW) # The target_sources() command converts relative paths to absolute.
cmake_policy(SET CMP0079 NEW) # target_link_libraries() allows use with targets in other directories.

add_subdirectory(src)
add_subdirectory(vendor)
