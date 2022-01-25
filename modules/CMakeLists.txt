cmake_minimum_required (VERSION 3.13.5)

project(${MODULES} LANGUAGES CXX C VERSION 0.1.1)

# Adding path for the project's include dirrectory.
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Current project source code folder.
add_subdirectory(source)
