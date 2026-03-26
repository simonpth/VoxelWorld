# Voxel World Documentation

## Multithreading 

Qt creates multiple Threads to handle its GUI
Here I create the EngineContext that creates the Engine and moves it to its own Thread
Inputs from the Qt Gui are then stored in the PlayerController in the engine-thread
Bevor each frame in the paint function the position of the player is updated from the rendering thread
When the player moves over the boarder of a chunk a signal is sent, 
which is received by the engine, which recalculates the chunks that should be rendered

This is noticed by the rendering thread which then will initiate the creation of the chunk and mesh generation.

## Program Execution & Threading

### Thread Overview

| Thread | Created By | Purpose |
|--------|-----------|---------|
| **Main Thread** | OS (`main()`) | GLFW window management, input polling, OpenGL rendering |
| **Engine Thread** | `EngineContext::createEngine()` | Game loop @50 ticks/sec, chunk management, player position updates |
| **Tick Threads** | `Engine::tick()` | Chunk mesh generation (spawned & joined per tick when player crosses chunk boundary) |

### Thread Interaction Flow

```mermaid
flowchart TD
    subgraph MainThread["🖥️ Main Thread"]
        A[main] --> B[EngineContext::createEngine]
        B --> C[App::initialize]
        C --> D[App::mainLoop]
        D --> E[renderer.processInput<br/>polls GLFW keys & mouse]
        E --> F[PlayerController::setInputState]
        F --> G[renderer.render]
        G --> H[PlayerController::update<br/>apply movement]
        H --> I[Read chunk vertices<br/>mutex-locked]
        I --> J[Render chunks]
        J --> K[glfwSwapBuffers]
        K --> L[glfwPollEvents]
        L --> E
        D --> M[App::cleanup]
        M --> N[EngineContext::deleteEngine]
    end

    subgraph EngineThread["⚙️ Engine Thread"]
        O[Engine::run] --> P[gameLoop]
        P --> Q{tick every 20ms}
        Q --> R{chunkChanged?}
        R -->|Yes| S[Spawn thread]
        R -->|No| Q
        S --> T[Join before next tick]
        T --> Q
    end

    subgraph TickThreads["🔄 Tick Threads"]
        U[ChunkManager::updateLoadedMeshes]
    end

    B -.->|spawns thread<br/>shared_ptr captured| O
    S -.->|spawns & joins| U
```

### Object Ownership

```mermaid
graph LR
    EC[EngineContext<br/>Singleton] -->|shared_ptr| E[Engine]
    E -->|owns| W[World]
    E -->|unique_ptr| CM[ChunkManager]
    E -->|unique_ptr| PC[RenderPlayerController]
    
    App[App] -->|owns| R[Renderer]
    R -->|map<string, ptr>| CRM[ChunkRenderMesh]
    
    CM -.->|mutex protected| CV[(ChunkVertices)]
    R -.->|reads with lock| CV
    
    style EC fill:#f9f,stroke:#333,stroke-width:2px
    style App fill:#bbf,stroke:#333,stroke-width:2px
```

### Cleanup Sequence

When the program exits, objects are destroyed in this order:

```mermaid
sequenceDiagram
    participant Main as 🖥️ Main Thread
    participant EC as EngineContext
    participant ET as ⚙️ Engine Thread
    participant E as Engine Objects
    
    Main->>App: ~App() destructor
    Main->>EC: deleteEngine()
    EC->>E: stop() → sets m_running = false
    Note over ET: Atomic flag detected,<br/>gameLoop exits
    EC->>ET: thread.join()
    ET-->>EC: thread finished
    EC->>EC: m_engine.reset()
    Note over E: Engine destroyed<br/>→ World destroyed<br/>→ ChunkManager destroyed<br/>→ PlayerController destroyed
    Main->>Main: return 0
```

### Synchronization Mechanisms

| Mechanism | Purpose |
|-----------|---------|
| `std::atomic<bool> m_running` | Signals engine thread to stop |
| `std::mutex` (ChunkVertices) | Protects chunk vertex data between engine and renderer threads |
| `std::shared_ptr<Engine>` | Ensures Engine stays alive while engine thread is running (ref count = 2) |

### Data Flow Summary

1. **Input**: Main thread polls GLFW input → updates `RenderPlayerController`
2. **Position Sync**: Before each frame, renderer reads player position from engine thread
3. **Chunk Detection**: Engine thread checks if player crossed chunk boundary
4. **Mesh Generation**: On boundary crossing, engine spawns tick thread to regenerate visible chunk meshes
5. **Rendering**: Main thread reads chunk vertices (mutex-protected) and renders

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

## Vertex Data Structure (uint64_t)

16 bit: block id

pos = coord / 8 => This allows for subvoxel detail meshes

8 bit: x \
8 bit: y \
8 bit: z

8 bit: width \
8 bit: height

5 bit: rotation

3 bit: free

### Rotation / Face

0-5 same as cube faces