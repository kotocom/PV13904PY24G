# Defining the "Generic" type for the build generator to use cross-compilation.
set(CMAKE_SYSTEM_NAME Generic)

# A extra information for the hardware target.
set(HW_ARCH "Armv7E-M")
set(HW_UNIT "STM32F756NGHx")
set(CMAKE_SYSTEM_VERSION "${HW_UNIT}_${HW_ARCH}")

# Target definitions.
set(TARGET_HW_FIRMWARE_FORMAT "ihex")
set(FORMAT_EXT "hex")

# By target it means the MCU the project should run on.
set(TARGET_HW "STM32F756NGHx")

# A set of configuration for the given target's hardware.
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(OBJCOPY arm-none-eabi-objcopy)

# CMAKE fails to run proper test with stdlib having _exit call stub, therefore we force it to not run the test.
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Architecture specific options. Overwriting flags to discard defaults.
set(COMMON_FLAGS "-mcpu=cortex-m7 -mlittle-endian -mthumb -DSTM32F756xG")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}" CACHE INTERNAL "")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}" CACHE INTERNAL "")

# Target programmer toolchain.
set(CMAKE_TARGET_HW_PROGRAMMER "ld_scripts/STM32F756NGHx_fla.sh" CACHE INTERNAL "")

# Target artifact memory usage.
set(CMAKE_ELFSIZE
    "arm-none-eabi-size" "--format=berkeley"
)

# Flags to delete  from compile_commands file in final post build cleanup.
set(POST_BUILD_CLEANUP "sed" "-i" "-E"
    "-e" "s/-mcpu=cortex-m7//g"
    "-e" "s/-mlittle-endian//g"
    "-e" "s/-mthumb//g"
    "-e" "s/-DSTM32F756xG//g"
    "-e" "s#/home/koto#$ENV{UPATH}#g"

    "${ROOT_CMAKE}/compile_commands.json"
)

# FW_PACKAGE Dependency.
set(TARGET_HW_EXT_LIBS_HASH "e96f9854")

# Set the linker script path.
set(TARGET_HW_LD_PATH "ld_scripts/${TARGET_HW}.ld")

# Set the linker script flags.
set(TARGET_HW_LD_CONFIG "-Wl,--gc-sections -lstdc++ -lm --specs=nosys.specs -Wl,-Map=final.map")
