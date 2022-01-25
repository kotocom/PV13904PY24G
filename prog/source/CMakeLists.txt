cmake_minimum_required (VERSION 3.13.5)

# Make a binary of the demo.
add_executable(${PROJECT_NAME}
    main.cpp
)

# Expose the include folders for the target.
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PROJECT_INCLUDE_DIR}
    ${STM32F7_INCLUDE}
    ${STM32F7_CMSIS_INCLUDE}
    ${MODULES_INCLUDE}
)

# Link against other libraries this executable includes.
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${MODULES}
)

# Getting and saving the path of the resulting file.
set(ARTIFACT_NAME "${PROJECT_NAME}_${TARGET_HW}.${FORMAT_EXT}")
set(ARTIFACT "${CMAKE_CURRENT_BINARY_DIR}/${ARTIFACT_NAME}" CACHE INTERNAL "")

# Converting the file type to the one we can write to target's memory.
if (EXISTS ${CMAKE_OBJCOPY})
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${OBJCOPY} -O ${TARGET_HW_FIRMWARE_FORMAT} $<TARGET_FILE:${PROJECT_NAME}>
                ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:${ARTIFACT_NAME}>
        COMMENT "Firmware file: ${CMAKE_CURRENT_BINARY_DIR}/${ARTIFACT_NAME} produced. ${Yellow}Hardware Target:${ColorReset} ${CMAKE_SYSTEM_VERSION}"
    )
else()
    message(WARNING "${Red}'objcopy' not found:${ColorReset} cannot generate ${ARTIFACT_NAME} file.")
endif()

# Getting statistics on memory usage.
if (CMAKE_ELFSIZE)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_ELFSIZE} ${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
    )
else()
    message(WARNING "${Red}'get_size' not found:${ColorReset} cannot print memory footprint.")
endif()
