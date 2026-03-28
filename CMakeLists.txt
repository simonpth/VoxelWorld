cmake_minimum_required(VERSION 3.20)
project(VoxelWorld VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source file
set(SOURCES
  src/main.cpp

  src/glfwrenderer/chunkrendermesh.cpp
  src/glfwrenderer/renderer.cpp
  src/glfwrenderer/app.cpp

  src/engine/algorithm/chunkgeneration.cpp
  src/engine/algorithm/chunkmeshing.cpp

  src/engine/data/chunkmanager.cpp
  src/engine/data/world.cpp

  src/engine/playercontroller/playercontroller.cpp

  src/engine/engine.cpp
  src/engine/enginecontext.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Fetch dependencies
include(FetchContent)

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

set(CMAKE_POLICY_VERSION_MINIMUM 3.20)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS   OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL      OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm)

target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw
  glm
)

# glad
target_sources(${PROJECT_NAME} PRIVATE lib/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE lib/glad/include)

# taskflow
target_include_directories(${PROJECT_NAME} PRIVATE lib)


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