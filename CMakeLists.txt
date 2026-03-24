cmake_minimum_required(VERSION 3.20)
project(VoxelWorld VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source file
set(SOURCES
    src/main.cpp

    src/glfwrenderer/chunkmesh.h
    src/glfwrenderer/chunkmesh.cpp
    src/glfwrenderer/shader.h
    src/glfwrenderer/shader.cpp
    src/glfwrenderer/window.h
    src/glfwrenderer/window.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

include(FetchContent)

set(CMAKE_POLICY_VERSION_MINIMUM 3.20)

# enkiTS - task scheduler
FetchContent_Declare(
  enkiTS
  GIT_REPOSITORY https://github.com/dougbinks/enkiTS.git
  GIT_TAG        v1.11
)

# GLFW - windowing
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)

# GLM - math
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.3
)

FetchContent_MakeAvailable(enkiTS glfw glm)

target_link_libraries(${PROJECT_NAME} PRIVATE
  enkiTS
  glfw
  glm
)

# glad
target_sources(${PROJECT_NAME} PRIVATE lib/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE lib/glad/include)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
)

# Copy shaders next to the executable after build.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework IOKit")
endif()