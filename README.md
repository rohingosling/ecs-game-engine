# ECS Game Engine - Particle Simulator

A lightweight Entity Component System (ECS) game engine written in C++17, demonstrated through an interactive particle physics simulator built with SDL2.

Originally developed in 2014 as an academic project exploring ECS architecture, this engine separates data from logic using entities, components, and systems rather than deep class hierarchies.

|  |  |
|---|---|
| ![Main Menu](demo/particle_demo/resources/screen%20shots/screenshot-menu-main-800.png) | ![Particle Simulation](demo/particle_demo/resources/screen%20shots/screenshot-simulation-800.png) |


## Demos

The project includes two demo applications:

### Hello World

A minimal console-only demo that demonstrates the core ECS workflow: registering components, creating entities, attaching data, and running a system loop.

### Particle Simulator

A graphical particle physics simulator featuring:

- **Newtonian gravity** - particles attract each other based on mass and distance
- **Short-range repulsion** - prevents particle overlap
- **Collision mechanics** - billiard-ball style elastic collisions with configurable iterations
- **Friction and elasticity** - tunable coefficients for realistic motion damping
- **Color-coded particle groups** - red, green, blue, and yellow particles with independent mass and radius
- **Interactive controls** - select and push individual particles with arrow keys
- **Particle trails** - color-coded motion trails with configurable depth and opacity
- **Menu system** - navigate settings, instructions, and about screens with keyboard controls

## Architecture

The engine is organized into three layers:

```
demo                        Application layer
├─ hello_world                Console-only ECS demo
└─ particle_demo              Graphical particle simulator
   ├─ engines                   EngineMenu, EngineParticleSimulator
   ├─ components                18 component types
   └─ systems                   9 system types

engine                      Engine utilities layer
├─ Engine.h                   Base game loop (frame rate, delta time, command flush)
├─ CommandManager.h           Deferred command queue, flushed each frame
├─ EventManager.h             String-keyed events with std::any payloads
├─ ResourceManager.h          Templated resource load/unload with key lookup
├─ GlobalCache.h              Global key-value store for cross-system data
├─ ApplicationSettings.h      INI-style settings parser
├─ math                       Vector2D, Vector3D (double-precision), GMath
└─ platform                   SDL2 wrappers (SDLWindow, SDLRenderer, SDLKeyboard)

ecs                         Core ECS framework
├─ World                      Central orchestrator: entities, components, systems
├─ Entity                     uint32_t alias (NULL_ENTITY=0, MAX_ENTITIES=4096)
├─ Signature                  std::bitset<64> for component membership
├─ ComponentArray<T>          Dense storage with sparse-set mapping
├─ ComponentManager           Type-indexed component registration
├─ EntityManager              Entity ID pool with recycling queue
└─ System                     Abstract base with update(World&, double dt)
```

### Why Three Layers?

The separation between `ecs/` and `engine/` is deliberate. The core ECS framework (`ecs/`) has **zero external dependencies** — it is pure C++17 with no reliance on SDL2 or any third-party library. This means `ecs/` can be dropped into any C++ project as a standalone ECS library, whether the host application uses SDL2, OpenGL, Vulkan, or runs headless on a server.

The engine utilities layer (`engine/`) builds on top of the ECS core, adding game-loop infrastructure, resource management, and platform abstractions. Within `engine/`, the `math/` and `platform/` directories are kept separate for the same reason: `math/` (Vector2D, Vector3D, GMath) is dependency-free and reusable anywhere, while `platform/` wraps SDL2 and carries that external dependency. Code that only needs vectors and math helpers can import from `math/` without pulling in SDL2.

The application layer (`demo/`) sits on top of both, wiring together ECS entities and engine services into runnable programs.

### ECS Flow

1. **Register components** with `world.registerComponent<T>()` - each type gets a unique bit position.
2. **Register systems** with `world.registerSystem<T>(name, signature)` - systems update in registration order.
3. **Create entities** with `world.createEntity()`, then `world.addComponent(entity, comp)` - entity signatures update automatically.
4. **Game loop** - each frame flushes deferred commands, calls `world.updateSystems(dt)`, swaps the render buffer, then regulates frame rate.

### Key Patterns

- **Deferred commands** - Structural changes (entity creation/destruction) during system updates go through `CommandManager` to avoid iterator invalidation.
- **Signature matching** - When an entity's component set changes, the `World` automatically adds or removes it from each system's entity set based on signature compatibility.
- **Multi-pass rendering** - Renderer systems iterate their entity sets in ordered passes (background, geometry, overlays, HUD).
- **Application state machine** - The particle demo orchestrates `EngineMenu` and `EngineParticleSimulator` via state transitions managed through `GlobalCache`.

## Controls

### Menu Navigation

| Key              | Action                                          |
|------------------|-------------------------------------------------|
| Up / Down Arrow  | Cycle through menu buttons                      |
| Left / Right Arrow | Adjust particle count on settings buttons     |
| Enter            | Activate the selected button                    |
| Esc              | Go back one menu level, or select Exit          |

### Simulation

| Key              | Action                                          |
|------------------|-------------------------------------------------|
| Arrow Keys       | Accelerate the selected particle                |
| Tab              | Select the next particle                        |
| Shift + Tab      | Select the previous particle                    |
| P                | Pause / unpause the simulation                  |
| T                | Toggle particle trails                          |
| Esc              | Deselect particle, or exit to menu              |

## Building

### Requirements

- CMake 3.16+
- C++17 compiler (GCC/MinGW recommended on Windows)
- SDL2, SDL2_image, SDL2_ttf (required only for particle_demo)

### MSYS2 / MinGW (Recommended on Windows)

Install dependencies via MSYS2 MINGW64 terminal:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf
```

### Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build build

# Build a specific target
cmake --build build --target hello_world
cmake --build build --target particle_demo
```

The `hello_world` target always builds. The `particle_demo` target only builds if SDL2, SDL2_image, and SDL2_ttf are found by CMake. If they are not found, CMake prints a status message and skips the target.

### VS Code

1. Install the **CMake Tools** and **C/C++** extensions.
2. Add `C:\msys64\mingw64\bin` to your system PATH.
3. Open the project folder and select the GCC kit when prompted.
4. Press **F7** to build or use the CMake status bar to select a target.

## Running

The particle demo executable can be run directly from the build directory or from a file explorer. A post-build step copies the `resources/` directory next to the executable, and `main.cpp` sets the working directory to the executable's location at startup, so relative resource paths resolve correctly regardless of how the application is launched.

```bash
./build/particle_demo
```

## Author

Rohin Gosling

## License

This project is provided for educational and academic purposes.
