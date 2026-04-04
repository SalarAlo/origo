
<p align="center">
  <img src="resources/logo.png" alt="Origo logo" width="140">
</p>

<h1 align="center">Origo</h1>

<p align="center">
  Origo is a Linux only 3D game engine written in C++23 and modern OpenGL.
</p>

---

## Design Choices and Goals

Origo is not intended to compete with established engines such as Unity or Godot.
It is a learning project developed for submission to the CV-Tag 2026 at the University of Koblenz.

---

## Showcase

<p align="center">
  <img src="resources/showcase/first.png" alt="Origo Showcase 1" width="48%">
  <img src="resources/showcase/second.png" alt="Origo Showcase 2" width="48%">
</p>

---

## Requirements

- **OS:** Linux  
  Developed and tested on Arch Linux

- **Compiler:** GCC with C++23 support

- **Graphics:** OpenGL  
  Proper driver support required (Mesa recommended)

- **Build system:** CMake + Ninja

---

## Clone and Build

The repository vendors its third-party dependencies directly, so a normal clone is enough:

```bash
git clone https://github.com/SalarAlo/origo.git
```

Then configure and build the project:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

---
  
## License

Licensed under the Apache License, Version 2.0.  
See the [LICENSE](LICENSE) and [NOTICE](NOTICE) files for details.
