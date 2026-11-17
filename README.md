# initOpenGL

A small C++ rendering sandbox built on modern (core-profile) OpenGL, written to learn how GPU rendering works from the ground up: buffers, vertex layouts, shaders, textures, transforms and an immediate-mode debug UI.

The app opens a window, draws a textured quad (a duck) through a custom shader, and lets you move it around in real time with an ImGui slider while showing frame time and FPS.

## Features

- **OpenGL 3.3 core profile** context created with GLFW, with function loading through GLEW
- **Thin C++ abstractions over raw OpenGL objects**, each owning its GPU resource (RAII):
  - `VertexBuffer` and `IndexBuffer` for vertex and index data on the GPU
  - `VertexBufferLayout` and `VertexArray` to describe and bind vertex attributes (position, texture coordinates)
  - `Shader` to parse, compile and link a single-file GLSL program, with a cached uniform-location lookup
  - `Texture` to load images with stb_image and upload them to texture slots
  - `Renderer` to clear the screen and issue indexed draw calls
- **Model-View-Projection transforms** with GLM (orthographic projection, view and model translation)
- **Alpha blending** for transparent textures
- **ImGui overlay** to adjust the model translation live and display frame time and FPS
- **OpenGL error checking** via a `GLCall()` macro that clears and checks `glGetError()` around each call and breaks into the debugger with the failing function, file and line

## Project structure

```
initOpenGL/
├── OpenGL.sln                  # Visual Studio solution
├── dependencies/
│   ├── GLEW/                   # GLEW headers and static libs
│   └── GLFW/                   # GLFW headers and libs (VS 2019)
└── OpenGL/
    ├── OpenGL.vcxproj
    ├── resources/
    │   ├── shaders/basic.shader   # vertex + fragment shader in one file
    │   └── textures/duck.png
    └── src/
        ├── Application.cpp        # window setup, scene and render loop
        ├── Renderer.*             # clear + draw calls
        ├── VertexBuffer.*         # GPU vertex data
        ├── IndexBuffer.*          # GPU index data
        ├── VertexArray.*          # binds buffers to attribute layouts
        ├── VertexBufferLayout.h   # attribute layout description
        ├── Shader.*               # shader parsing, compilation, uniforms
        ├── Texture.*              # image loading and texture binding
        ├── ErrorHandling.*        # GLCall error-checking macro
        └── vendor/                # glm, imgui (1.60), stb_image
```

## Shaders

Both shader stages live in one file, `resources/shaders/basic.shader`, separated by `#shader vertex` and `#shader fragment` markers. `Shader` splits the file, compiles each stage and links the program.

- The vertex shader transforms each vertex by the `u_MVP` matrix and passes texture coordinates on.
- The fragment shader samples `u_Texture` and outputs its color.

## Building and running

The project targets **Windows** with **Visual Studio 2019** (toolset v142). All dependencies are included in the repository, so no separate installs are needed.

1. Clone the repository.
2. Open `OpenGL.sln` in Visual Studio 2019 or later (retarget the toolset if Visual Studio asks).
3. Select the **Debug | x86** configuration. It is the one set up with the include path for `src/vendor`.
4. Build and run. The working directory must be the `OpenGL/` project folder so the shader and texture paths under `resources/` resolve (this is the Visual Studio default).

You need a GPU and driver that support OpenGL 3.3 or later. The OpenGL version in use is printed to the console at startup.

## Controls

| Control | Effect |
| --- | --- |
| **Model Translation** slider | Moves the quad along x, y and z |
| Frame stats | Shows average frame time (ms) and FPS |

## Libraries used

- [GLFW](https://www.glfw.org/) for windows, input and the OpenGL context
- [GLEW](http://glew.sourceforge.net/) for loading OpenGL functions
- [GLM](https://github.com/g-truc/glm) for vector and matrix math
- [Dear ImGui](https://github.com/ocornut/imgui) (v1.60) for the debug UI
- [stb_image](https://github.com/nothings/stb) for loading textures
