cmake_minimum_required(VERSION 3.22)

project(specified_project)
add_library(specified_project INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(specified_project INTERFACE 
    USE_HAL_DRIVER 
    STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(specified_project INTERFACE
    include
    ${LIB_DIR}/CMSIS/Core/Include
    ${LIB_DIR}/CMSIS/Device/ST/STM32F1xx/Include
    ${LIB_DIR}/STM32F1xx_HAL_Driver
    ${LIB_DIR}/STM32F1xx_HAL_Driver/Inc
)

target_sources(specified_project INTERFACE
    src/main.c
    src/startup_stm32f10x_md.s
)

target_link_directories(specified_project INTERFACE
)

target_link_libraries(specified_project INTERFACE
    STM32F1xx_HAL_Driver
)

# Validate that specified_project code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


