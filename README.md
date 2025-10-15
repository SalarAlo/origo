# Origo

Origo is a 3D engine targeting Linux (only) developed by me and only me.

![LOGO](resources/textures/origo_logo.png)

---

## What it is

- A barebones engine written in C++ & GLSL  
- Uses CMake for build  
- Linux-only windowing, input, rendering backends  
- Focus: correctness, clarity, control, and performance

---

_(The engine is under active development — not everything is stable.)_

---

## Requirements & Dependencies

- Linux (optimally on Fedora as I develop it on Fedora)  
- A decent C++ compiler (supporting C++20 or newer)  
- OpenGL drivers available  
- CMake  
- (Optionally) external libs you include (e.g. stb_image, GLM, etc.)

---

## Layout

origo/ → engine core code
<br>
game/ → sample/demo project
<br>
resources/ → assets (shaders, textures, models)
<br>
vendor/ → third-party libs included
<br>
CMakeLists.txt → build script
<br>
pch.* → precompiled headers
<br>
overview.sh → helper script
