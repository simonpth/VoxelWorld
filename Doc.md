# Voxel World Documentation

## Multithreading 

Qt creates multiple Threads to handle its GUI
Here I create the EngineContext that creates the Engine and moves it to its own Thread
Inputs from the Qt Gui are then stored in the PlayerController in the engine-thread
Bevor each frame in the paint function the position of the player is updated from the rendering thread
When the player moves over the boarder of a chunk a signal is sent, 
which is received by the engine, which recalculates the chunks that should be rendered

This is noticed by the rendering thread which then will initiate the creation of the chunk and mesh generation.

## Cube Faces

| n |face|
|---|----|
| 0 | +x |
| 1 | +y |
| 2 | +z |
| 3 | -x |
| 4 | -y |
| 5 | -z |

## Traversing Order (most inner loop first)

x -> z -> y