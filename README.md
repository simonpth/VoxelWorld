# VoxelWorld

A fun voxel-based world engine built with GLFW and OpenGL.

## About

VoxelWorld is a solo developer project exploring voxel rendering and world generation using modern C++ (C++23). This project serves as both a learning experience and a foundation for potential game development.

## Features

- Exploring a procedurally generated voxel world
- Dynamic chunk loading and unloading based on player position
- Possible even on older hardware as I optimized everything for performance and memory usage

## Technical Stack

- C++23
- Graphics: OpenGL 4.1 Core Profile + Custom GLSL Shaders + GLFW
- FastNoise2 for noise
- taskflow for efficient multithreading

## Getting Started

Download the latest release from the [Releases](https://github.com/simonpth/VoxelWorld/releases) page or clone the repository and build it yourself (see below).

## Building and Running

This allows you to experiment with the chunk generation and rendering. You can modify the node tree in `src/engine/algorithm/chunkgeneration.cpp` to see how it affects the terrain (see the FastNoise2 WebEditor)

Make sure you have a working C++23 compiler and CMake installed. Then follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/simonpth/VoxelWorld.git
   ```

2. Navigate to the project directory:

   ```bash
   cd VoxelWorld
   ```

3. Build the project using CMake:

   Linux/Mac:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

   clang on Windows
   ```bash
   cmake -S . -B build -T ClangCL
   cmake --build build
   ```

4. Run the executable:

    Linux/Mac:
    ```bash
    ./VoxelWorld
    ```
  
    Windows:
    ```bash
    VoxelWorld.exe
    ```

## Licence

GNU GENERAL PUBLIC LICENSE Version 3
