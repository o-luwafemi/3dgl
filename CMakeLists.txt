cmake_minimum_required(VERSION 3.12.0)
project(3dgl VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)


add_executable(3dgl
    
)

target_sources(3dgl
  PRIVATE
    src/main.cpp
    src/glad.c
)

target_include_directories(3dgl
    PRIVATE
    dependencies/include
    src/include

)

target_link_libraries(3dgl
    # dependencies/library
    glfw
    OpenGL::GL
    assimp
)