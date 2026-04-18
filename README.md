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

## Optimizations

This project would not have been possible without optimizing a lot of aspects.
Here is a few things I did to make it possible:

### Data Structure

- The world is split into chunks
  - every chunk is 32x32x32
  - the data of every chunk is put into 8x8x8 pieces
  - if a piece consists of the same block, only this block is stored (huge RAM savings)
- The chunks are stored in an unordered hash map for fast access (O(1))
- Chunks are only generated when needed
  - chunk generation uses FastNoise2 which uses FastSIMD, which can accelerate chunk generation by a lot as it can bulk generate the height values

### Rendering

- Chunks are face culled using a binary algorithm

```cpp
uint64_t mask = meshingData->xSolidMasks[index];
uint64_t maskP = mask & ~(mask >> 1);
uint64_t maskN = mask & ~(mask << 1);

while (maskP != 0) {
  int x = std::countr_zero(maskP) - 1;
  if (x >= 0 && x < Chunk::SIZE)
    faceVertices[0].push_back(generateVertex(chunk->block(x, y, z).id,
                                              x * 8, y * 8, z * 8, 8, 8, 0));
  maskP &= (maskP - 1);
}
while (maskN != 0) {
  int x = std::countr_zero(maskN) - 1;
  if (x >= 0 && x < Chunk::SIZE)
    faceVertices[3].push_back(generateVertex(chunk->block(x, y, z).id,
                                              x * 8, y * 8, z * 8, 8, 8, 3));
  maskN &= (maskN - 1);
}
```

- Face culling is done every frame so the engine only draws what it needs to draw

- The vertex data is stored in a custom format that is far more compact than using singular vertices with each having a position normal and uv:
  - for every face there only is 64bit:
    - 16 bit: block id

    pos = coord / 8 => This allows for subvoxel detail meshes:
    - 8 bit: x
    - 8 bit: y
    - 8 bit: z
    - 8 bit: width
    - 8 bit: height
    - 5 bit: rotation
    - 3 bit: free
  - every vertex is used 4 times using:
    ```cpp
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_currentVerticesSize);
    ```
  - uvs and normals are constructed out of this data

- the rendering thread only uploads meshes if they are ready and renders them, everything else happens on the engine thread, which handles starting the chunk generation and meshing process and anything not rendering related

- To avoid checking if the chunk manager has new chunks every frame there is a queue the chunk manager writes to what he changed and the rendering thread only needs to check this

- The objects that take care of the chunk mesh uploading and rendering store a version of their mesh so they only upload a new mesh when it changed in the data structre. This also makes sure that they do not go back in time if an older update finishes later than a newer one.

### Overall

- Multithreading is a core part of the engine and is used for chunk generation, meshing and rendering. This allows the engine to utilize multiple CPU cores and keep the main thread responsive.

- Data structures are always selected with performance in mind so you will find quite a few unordered hash maps for long term storage and vectors when they are only used for some algorithms. I also always tried to reserve memory for the vectors when possible to avoid unnecessary reallocations.

### Conclusion

Without these optimizations the project just would fall apart and not be playable and explorable at all.

## Licence

GNU GENERAL PUBLIC LICENSE Version 3
