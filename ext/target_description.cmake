# System include paths.
set(STM32F7_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/${FW_PACKAGE_PATH}/Drivers/CMSIS/Device/ST/STM32F7xx/Include" CACHE INTERNAL "")
set(STM32F7_CMSIS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/${FW_PACKAGE_PATH}/Drivers/CMSIS/Include" CACHE INTERNAL "")
#set(RTOS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/include" CACHE INTERNAL "")

## Credits.
set(FW_PACKAGE "https://github.com/STMicroelectronics/STM32CubeF7")
message(STATUS "From: ${Blue}${FW_PACKAGE}${ColorReset}")

# Get the current working branch
execute_process(
  COMMAND git -C ${STM32F7_INCLUDE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE FW_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git -C ${STM32F7_INCLUDE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE FW_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT ${TARGET_HW_EXT_LIBS_HASH} STREQUAL ${FW_GIT_COMMIT_HASH})
    message(STATUS "${Yellow}Target HW require different commit hash, switching to ${TARGET_HW_EXT_LIBS_HASH}${ColorReset}")
    execute_process(
        COMMAND bash "-c" "git -C ${STM32F7_INCLUDE} checkout -f ${TARGET_HW_EXT_LIBS_HASH} 2> >(sed -e 's/^\\(.\\{5\\}\\).*/\\1/')"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE COMMAND_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if("${COMMAND_RESULT}" STREQUAL "error")
        message(FATAL_ERROR "${Red} Checkout to ${TARGET_HW_EXT_LIBS_HASH} failed.${ColorReset}")
    endif()
else()
    message(STATUS "${Green}Using STM32F7_CMSIS From branch: ${FW_GIT_BRANCH} on commit: ${FW_GIT_COMMIT_HASH}${ColorReset}")
endif()
