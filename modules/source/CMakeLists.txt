cmake_minimum_required (VERSION 3.13.5)

# Use the source file to generate the lib.a
set(source_files
    startup_stm32f756xx.S
    system.c
    display.cpp
    spi.cpp
    draw.cpp
)

# Force cmake to deliver STATIC library only.
add_library(${PROJECT_NAME} STATIC ${source_files})

# Expose the include folders for the lib.
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_INCLUDE_DIR}
    ${STM32F7_INCLUDE}
    ${STM32F7_CMSIS_INCLUDE}
)

