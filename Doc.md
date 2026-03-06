# Voxel World Documentation

## Multithreading - AI Generated for now

The Voxel World application uses a multi-threaded architecture to handle different aspects of the game engine efficiently. Here's an overview of the threading model:

### Explicit Threads Created

1. **Game Loop Thread**: The main game loop runs in a separate thread created using `QThread::create()` in the `Engine::run()` method. This thread handles:
   - Game logic updates (physics, AI, world updates)
   - Timing and synchronization using a fixed tick rate (20ms per tick)
   - Sleep management to maintain consistent frame rates

### Qt Default Threads

The application leverages several default Qt threads:

1. **Main GUI Thread**: This is the primary thread where:
   - The QGuiApplication runs (`main.cpp`)
   - QML UI components are managed
   - User input events are processed
   - OpenGL rendering is coordinated

2. **QML/JavaScript Thread**: Qt Quick uses this thread for:
   - QML component management
   - Property bindings and animations
   - JavaScript execution

3. **OpenGL Rendering Thread**: The GLQuickItem and GLRenderer operate in a rendering thread that:
   - Handles OpenGL context management
   - Executes the `paint()` method for frame rendering
   - Manages the OpenGL state and shaders

### Thread Synchronization

The application uses several synchronization mechanisms:

1. **QMutex**: Used in the Engine class (`m_gameLoopMutex`) to ensure only one game loop exists
2. **QMutexLocker**: Used in PlayerController for thread-safe input handling (`m_inputMutex`)
3. **Atomic Variables**: `std::atomic<bool>` is used for thread-safe boolean flags like `m_running`
4. **Qt Signal/Slot System**: Used for cross-thread communication (e.g., `chunkChanged` signal)

### Thread Communication Patterns

1. **Signal-Slot Connections**: Qt's signal-slot mechanism handles cross-thread communication automatically
2. **Shared Data with Mutex Protection**: Input state and game state are protected by mutexes when accessed from multiple threads
3. **Atomic Flags**: Used for thread-safe boolean state management

### Performance Considerations

1. The game loop thread runs at a fixed 50Hz tick rate (20ms per tick)
2. Rendering happens independently in the OpenGL thread
3. Input handling occurs in the main GUI thread and is synchronized with the game loop
4. The architecture avoids blocking the main thread to maintain UI responsiveness

This multi-threaded design allows the voxel world to maintain smooth gameplay while handling complex world updates, rendering, and user input simultaneously.

