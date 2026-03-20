cmake_minimum_required(VERSION 3.20)
project(VoxelWorld VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source file
set(SOURCES
    src/main.cpp

    src/glfwrenderer/window.h
    src/glfwrenderer/window.cpp
    src/glfwrenderer/shader.h
    src/glfwrenderer/shader.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# glad
target_sources(${PROJECT_NAME} PRIVATE lib/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE lib/glad/include)


# Link libraries
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)

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