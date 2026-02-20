cmake_minimum_required(VERSION 3.16)

project(VoxelWorld VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick)

find_package (Vulkan REQUIRED)

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
        vulkanwindow.h vulkanwindow.cpp
)

set_target_properties(appVoxelWorld PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appVoxelWorld
    PRIVATE Qt6::Quick
)

target_link_libraries(appVoxelWorld PRIVATE Vulkan::Vulkan)

include(GNUInstallDirs)
install(TARGETS appVoxelWorld
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
