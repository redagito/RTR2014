# Screenshots

![Screen1](https://github.com/redagito/RTR2014/wiki/screenshots/screen1.png)
![Screen2](https://github.com/redagito/RTR2014/wiki/screenshots/screen2.png)
![Screen3](https://github.com/redagito/RTR2014/wiki/screenshots/screen3.png)
![Screen4](https://github.com/redagito/RTR2014/wiki/screenshots/screen4.png)
![Screen5](https://github.com/redagito/RTR2014/wiki/screenshots/screen5.png)
![Screen6](https://github.com/redagito/RTR2014/wiki/screenshots/screen6.png)
![Screen7](https://github.com/redagito/RTR2014/wiki/screenshots/screen7.png)
![Screen8](https://github.com/redagito/RTR2014/wiki/screenshots/screen8.png)
![Screen9](https://github.com/redagito/RTR2014/wiki/screenshots/screen9.png)
![Screen10](https://github.com/redagito/RTR2014/wiki/screenshots/screen10.png)

# Introduction

This demo was created for the realtime rendering university course. It features a data driven deferred OpenGL based renderer with post processing capabilities.

The original version of this project, before the build system migration described below, is preserved at the [`v1`](https://github.com/redagito/RTR2014/tree/v1) tag.

## Features
* Modern OpenGL (4.1 core profile)
* Cross platform (developed on Windows 10)
* Deferred rendering
* Point lights
* Directional lights
* Normal mapping
* Shadow maps
* Omnidirectional shadows
* Depth of field effect
* FXAA
* Screen space godray effect (experimental)

# Building

All third party dependencies are resolved through Conan 2, so there are no git submodules to clone.

The OpenGL loader is [glad](https://github.com/Dav1dde/glad), which ships pregenerated for the GL 4.1 core profile. The previous flextGL codegen step is gone.

## Prerequisites

* CMake 3.28 or newer
* Conan 2
* A C++23 capable compiler

## Debug build

```
conan install . --build=missing -s build_type=Debug
cmake --preset conan-default
cmake --build build --config Debug
```

Output:

```
build/Debug/RTRDemo.exe
build/Debug/data/
```

## Release build

```
conan install . --build=missing -s build_type=Release
cmake --preset conan-default
cmake --build build --config Release
```

Output:

```
build/Release/RTRDemo.exe
build/Release/data/
```

Verified on Windows with CMake 4.4.3, Conan 2.32.0 and MSVC 19.51.
