cmake_minimum_required(VERSION 3.16)
project(ECSGameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Core ECS library (header-only + World.cpp).
# ---------------------------------------------------------------------------

add_library(ecs STATIC
    ecs/World.cpp
)

target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------------------------
# HelloWorld demo (console only, no SDL2).
# ---------------------------------------------------------------------------

add_executable(hello_world
    demo/hello_world/main.cpp
)

target_link_libraries(hello_world PRIVATE ecs)

# ---------------------------------------------------------------------------
# SDL2 platform layer + ParticleDemo (only if SDL2 is found).
# ---------------------------------------------------------------------------

find_package(SDL2 QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_ttf QUIET)

if(SDL2_FOUND AND SDL2_image_FOUND AND SDL2_ttf_FOUND)

    add_executable(particle_demo
        demo/particle_demo/main.cpp
        demo/particle_demo/Application.cpp
        demo/particle_demo/engines/EngineMenu.cpp
        demo/particle_demo/engines/EngineParticleSimulator.cpp
        engine/platform/SDLWindow.cpp
        engine/platform/SDLRenderer.cpp
        engine/platform/SDLKeyboard.cpp
    )

    target_link_libraries(particle_demo PRIVATE
        ecs
        SDL2::SDL2main
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        dwmapi
    )

    target_include_directories(particle_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Copy resources next to the executable so relative paths work from any cwd.
    add_custom_command(TARGET particle_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/demo/particle_demo/resources
        $<TARGET_FILE_DIR:particle_demo>/resources
    )

else()
    message(STATUS "SDL2/SDL2_image/SDL2_ttf not found - skipping particle_demo")
endif()
