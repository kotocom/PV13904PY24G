cmake_minimum_required (VERSION 3.13.5)

# We need colors!
string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(ColorBold  "${Esc}[1m")
set(Red         "${Esc}[31m")
set(Green       "${Esc}[32m")
set(Yellow      "${Esc}[33m")
set(Blue        "${Esc}[34m")
set(Magenta     "${Esc}[35m")
set(Cyan        "${Esc}[36m")
set(White       "${Esc}[37m")
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")


# Fetch all the submodules if needed.
execute_process(
    COMMAND bash "-c" "git submodule status | awk '{FNR <= 1} END { if (NF > 2) print \"TRUE\" }'"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE SUBMODULES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (SUBMODULES STREQUAL "TRUE")
    execute_process(
        COMMAND bash "-c" "git submodule status"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE SUBMODULES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Using submodules:\n${Green}${SUBMODULES}${ColorReset}")
else()
    message(WARNING "Submodules not loaded: ${Yellow}pulling${ColorReset}")
    execute_process(
        COMMAND bash "-c" "git submodule update --init --recursive"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()


# Printing build type status message.
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "" FORCE)
    message(STATUS "Build type: ${Yellow}${CMAKE_BUILD_TYPE}${ColorReset}, use: -DCMAKE_BUILD_TYPE=Debug if necessary.")
# Screening wrong values.
elseif (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "Release")
    message(STATUS "Build type: ${Yellow}${CMAKE_BUILD_TYPE}${ColorReset}")
else()
    message(FATAL_ERROR "Build type: >>> ${CMAKE_BUILD_TYPE} <<<${Red} Only Release and Debug are legit. (Case sensitive)${ColorReset}")
endif()

set(PROJECT_ALIAS "prog")

## Toolchain file needs to be included before project() definition.
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ALIAS}/toolchain.cmake)

# List the languages the project based on, so the compiler doesn't get confused.
project(${PROJECT_ALIAS} LANGUAGES CXX C ASM VERSION 0.1.1)

# Using -std=c99
set(CMAKE_C_STANDARD 99)

# Using -std=c++17
set(CMAKE_CXX_STANDARD 17)

# Search for programs in the build host directories.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Libraries, packages and include file locations are within the project folder hierarchy.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Adding export of all files compile commands to generate compile_commands.json
# file that is used by a toolchain's language server daemon.
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

## Automatically copy the compile_commands.json to the current root for the language daemon.
set(COMPILE_COMMANDS_FILE "compile_commands.json")

# Specifying root cmake path for internal use.
set(ROOT_CMAKE "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "" FORCE)

add_custom_target(copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${COMPILE_COMMANDS_FILE} ${ROOT_CMAKE}
    COMMENT "${ROOT_CMAKE}/${COMPILE_COMMANDS_FILE} generated."
)

# Apply strict compiler flags.
add_compile_options(-Werror -Wall -Wextra -Wconversion -Wpedantic)

# Add --verbose beforehand if "DEBUG" stated.
add_link_options(
    $<$<CONFIG:DEBUG>:--verbose>
)

# To propagate the custom linker script for all components.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_SOURCE_DIR}/${TARGET_HW_LD_PATH} ${TARGET_HW_LD_CONFIG}")

add_compile_options(
    "-fno-exceptions"
    $<$<CONFIG:DEBUG>:--verbose>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-g>
    # Propagate DEBUG definition for all source files.
    $<$<CONFIG:DEBUG>:-DDEBUG=1>
    $<$<CONFIG:RELEASE>:-Os>
)

# HW System includes.
set(FW_PACKAGE "STM32CubeF7")
set(FW_PACKAGE_PATH "ext/${FW_PACKAGE}")
set(PACKAGE_DESCRIPTION "${CMAKE_CURRENT_SOURCE_DIR}/ext/target_description.cmake")
include(${PACKAGE_DESCRIPTION})

# System modules we use.
set(MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")
set(MODULE_DESCRIPTION "${MODULES_PATH}/target_description.cmake")
include(${MODULE_DESCRIPTION})

# Current project source code folder.
add_subdirectory(${PROJECT_NAME})

# Current project modules source code folder.
add_subdirectory(${MODULES_PATH})

if(CMAKE_TARGET_HW_PROGRAMMER)
    add_custom_target(run
        COMMAND sudo bash "-c" "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_TARGET_HW_PROGRAMMER} ${ARTIFACT}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Flashing target ..."
        VERBATIM
    )
else()
    message("${Yellow}-- WARNING: Run on target not available.${ColorReset} Define: CMAKE_TARGET_HW_PROGRAMMER toolchain.")
endif()

# Using doxygen & graphviz
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "${Red}Doxygen is needed to build the documentation.${ColorReset}")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    message("Doxygen build started.")

    add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM
    )
endif()
