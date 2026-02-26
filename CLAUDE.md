# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from project root)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build build

# Build a specific target
cmake --build build --target hello_world
cmake --build build --target particle_demo
```

The `hello_world` target always builds. The `particle_demo` target only builds if SDL2, SDL2_image, and SDL2_ttf are found via `find_package`.

There is no test infrastructure in this project.

## Architecture

### Layer Diagram

```
demo/                          Application layer
  hello_world/                   Console-only ECS demo
  particle_demo/                 Graphical particle simulator
    engines/                       EngineMenu, EngineParticleSimulator
    components/                    23 component types
    systems/                       9 system types

engine/                        Engine utilities layer
  Engine.h                       Base game loop (frame rate, dt, command flush)
  CommandManager.h               Deferred command queue, flushed each frame
  EventManager.h                 String-keyed events with std::any payloads
  ResourceManager.h              Templated resource load/unload with key lookup
  GlobalCache.h                  Global key-value store (std::any) for cross-system data
  ApplicationSettings.h          INI-style settings parser
  math/                          Vector2D, Vector3D (double-precision), GMath
  platform/                      SDL2 wrappers (SDLWindow, SDLRenderer, SDLKeyboard)

ecs/                           Core ECS framework
  World                          Central orchestrator: creates entities, registers
                                 components/systems, runs update loop
  Entity                         uint32_t alias; NULL_ENTITY=0, MAX_ENTITIES=4096
  Signature                      std::bitset<64> for component membership
  ComponentArray<T>              Dense storage with sparse-set mapping (swap-and-pop)
  ComponentManager               Registers component types, maps via std::type_index
  EntityManager                  Entity ID pool with recycling queue
  System                         Abstract base: update(World&, double dt), entity set
```

### ECS Flow

1. **Register components** with `world.registerComponent<T>()` — each type gets a unique bit position.
2. **Register systems** with `world.registerSystem<T>(name, signature)` — systems update in registration order.
3. **Create entities** with `world.createEntity()`, then `world.addComponent(entity, comp)` — the entity's signature is updated and system entity sets are refreshed automatically.
4. **Game loop** (`Engine::run`): each frame flushes `CommandManager`, calls `world.updateSystems(dt)`, swaps the render buffer, then regulates frame rate.

### Key Patterns

- **Deferred commands**: Structural changes (entity creation/destruction) during system updates go through `CommandManager` to avoid iterator invalidation. Commands are flushed at the start of each frame.
- **Signature matching**: When an entity's component set changes, `World::updateSystemEntitySets` checks every system's signature and adds/removes the entity from its `entities` set.
- **Multi-pass rendering**: `SystemRenderer` and `SystemMenuRenderer` iterate their entity sets multiple times in ordered passes (background → geometry → overlays → HUD).
- **Application state machine**: `particle_demo/Application` owns a `GlobalCache` and orchestrates two engines (`EngineMenu`, `EngineParticleSimulator`) via state transitions (IDLE → MENU_MAIN → LEVEL_PARTICLE_SIMULATION, etc.).
- **ComponentWorld singleton**: A single entity holds a `ComponentWorld` with bool flags (gravity, friction, elasticity, etc.) that systems read each frame to toggle behaviors.

## Naming Conventions

- Components: `ComponentFoo` (e.g., `ComponentPhysics`, `ComponentTransform`)
- Systems: `SystemFoo` (e.g., `SystemCollider`, `SystemGravity`)
- Engines: `EngineFoo` (e.g., `EngineMenu`, `EngineParticleSimulator`)
- Namespaces: `ecs` for core framework, `engine` for utilities/platform
- Header guards: `#pragma once`
- Brace style: Allman (opening brace on its own line)

## Lessons Learned and Gotchas

- **ECS initialization order matters**: Systems must be registered *before* entities are created. When `world.addComponent()` is called, it triggers `World::updateSystemEntitySets` which only populates *already-registered* systems. If entities are created first, system entity sets remain empty. Correct order: register components → register systems → create entities → configure systems with entity references.
- **Renderer skips disabled entities**: Disabled buttons/text boxes (`ComponentButtonState.enabled = false`) must be skipped entirely in the renderer, not drawn with a "disabled" appearance, because multiple screens share the same positional slots.
- **Resource paths are relative to the working directory**: The exe expects `resources/` in its working directory. A CMake post-build step copies `demo/particle_demo/resources/` next to the executable, and `main.cpp` sets the cwd to the exe's directory at startup.
- **System font dependencies**: The text box renderer uses `cour.ttf` (Courier New), which is a Windows system font not copied to `resources/Fonts/`.

## Build Requirements

- CMake 3.16+
- C++17 compiler
- SDL2, SDL2_image, SDL2_ttf (optional, required only for particle_demo)
