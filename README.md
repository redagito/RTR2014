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

The original version of this project is preserved at the [`v1`](https://github.com/redagito/RTR2014/tree/v1) tag.

## Features
* Deferred rendering
* Point lights
* Directional lights
* Normal mapping
* Shadow maps
* Omnidirectional shadows
* Depth of field effect
* FXAA

# Building

## Prerequisites

* CMake 3.28 or newer
* Conan 2
* A C++23 capable compiler
* An OpenGL 4.6 capable GPU and driver

## Debug build

```
conan install . --build=missing -s build_type=Debug
cmake --preset conan-default
cmake --build build --config Debug
```

## Release build

```
conan install . --build=missing -s build_type=Release
cmake --preset conan-default
cmake --build build --config Release
```

# Running

The demo reads its data from the `data` folder, it is started from the project directory or from the directory the data was copied to.

```
build/App/Release/RTRDemo.exe
```

## Capturing a frame

The capture mode renders a number of frames without showing the window and writes the last one to a PNG file. The frames are rendered with a fixed time step, so that the same image is written every time.

```
build/App/Release/RTRDemo.exe --capture shot.png --frames 2400
```

The number of frames selects the point on the camera sequence of `data/democam.json` that is captured, one frame is a sixtieth of a second.
