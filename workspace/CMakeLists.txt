
# 要求的最小版本号
cmake_minimum_required(VERSION 3.22)

# 设置编译器
set(CMAKE_C_STANDARD 99)            # c标准
set(CMAKE_C_STANDARD_REQUIRED ON)   # 指明要求支持c标准
set(CMAKE_C_EXTENSIONS ON)          # c扩展(暂时不知道干啥的)

# 设置构建类型(默认调试模式)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 要编译的项目
message("building userproj ${USER_PROJ}")
# 设置项目名称
set(USER_PROJECT ${USER_PROJ})
# set(CMAKE_PROJECT_NAME CACHE STRING ${p}) # 好像传参进来后面会消失
set(CMAKE_PROJECT_NAME ${USER_PROJECT})
# 设置项目信息
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

message("CMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME}")

# 包含用户配置文件
# include("cmake/gcc-arm-none-eabi.cmake")
set(WORKSPACE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include ("projects/${USER_PROJECT}/user.cmake")
include ("cmake/config_parse.cmake")

# 导出编译过程用到的指令
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 使能c语言和汇编
enable_language(C ASM)

# 可执行文件
add_executable(${CMAKE_PROJECT_NAME})

# 添加子目录
add_subdirectory(projects/${CMAKE_PROJECT_NAME})

# 置链接库目录
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# 添加源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# 添加包含目录
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# 添加编译时宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# 添加链接库
target_link_libraries(${CMAKE_PROJECT_NAME}
    specified_project

    # Add user defined libraries
)
