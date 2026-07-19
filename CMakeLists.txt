cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# WARN: cannot use `-sanitize=address` since TBB will error at exit.
# TBB is from libpagmo.dylib
set(CMAKE_C_FLAGS   "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Prevent macOS host arch flags from leaking in
set(CMAKE_OSX_ARCHITECTURES "")
set(CMAKE_OSX_SYSROOT "")

# STM32H7 core flags
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb")

# If your exact H7 variant has FPU enabled, also use:
set(FPU_FLAGS "-mfpu=fpv5-d16 -mfloat-abi=hard")

set(COMMON_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -ffunction-sections -fdata-sections")

set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS} ${FPU_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(as5600 LANGUAGES C VERSION 0.1)

####################################################################################################
### MAIN
####################################################################################################

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE SRCS "${SRC_DIR}/*.c")

add_executable(${PROJECT_NAME}
    ${SRCS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32H753xx
    USE_HAL_DRIVER
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32H7xx_Nucleo
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
)

# target_link_directories(${PROJECT_NAME}
#     PRIVATE
# )

# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
# )

# set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
