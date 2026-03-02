cmake_minimum_required(VERSION 3.20)

project(VoxelWorld VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appVoxelWorld
    main.cpp
)

qt_add_qml_module(appVoxelWorld
    URI VoxelWorld
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        engine/engine.h engine/engine.cpp
        engine/objectengine.h engine/objectengine.cpp
        engine/world.h engine/chunk.h engine/block.h
        
        glrenderer/glquickitem.h glrenderer/glquickitem.cpp
        glrenderer/glrenderer.h glrenderer/glrenderer.cpp
    RESOURCES
        shaders/shader.vert
        shaders/shader.frag
)

set_target_properties(appVoxelWorld PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appVoxelWorld
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml 
        Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appVoxelWorld
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
