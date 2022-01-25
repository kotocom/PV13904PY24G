cmake_minimum_required (VERSION 3.13.5)

# Adding path for the project's include directory.
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Adding the directory with the project's source code.
add_subdirectory(source)
