# GFXEngineGameTemplate

A modern C++ game template project built on top of **GFXEngine**.

## Overview

This repository provides a minimal starting point for creating a 3D game using GFXEngine.  
It includes:

- A basic game entry point
- A `MyGame` class with lifecycle callbacks (`onInit`, `onUpdate`, `onRender`, etc.)
- Offscreen rendering with a present pass
- UI context initialization
- Automatic shader copy step after build

## Requirements

- CMake 3.10 or newer
- A C++20-compatible compiler
- Vulkan-capable GPU and drivers
- Visual Studio / MSVC (recommended on Windows)

## Build

From the repository root:

1. Configure:
   - `cmake -S . -B out/build -G Ninja`
2. Build:
   - `cmake --build out/build`

> During configuration, CMake fetches `GFXEngine` via `FetchContent`.

## Run

After building, start the generated executable for `GFXEngineGameTemplate` from your build output directory.

## Project Structure

- `CMakeLists.txt` — root CMake configuration and engine dependency setup
- `GFXEngineGameTemplate/CMakeLists.txt` — target configuration for the game executable
- `GFXEngineGameTemplate/GFXEngineGameTemplate.cpp` — application entry point
- `GFXEngineGameTemplate/MyGame.h/.cpp` — game logic and render/update lifecycle

## Notes

- The project copies engine shaders to the output directory automatically after build.
- The template is intended as a starting point and can be extended with your own scenes, assets, and gameplay systems.
