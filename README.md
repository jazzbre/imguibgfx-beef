# [imguibgfx-beef](https://github.com/jazzbre/imguibgfx-beef) ImGui wrapper (with built-in Bgfx/SDL2 support) for the Beef Programming Language

[Dear ImGui](https://github.com/ocornut/imgui)

## Dependencies
[cimgui](https://github.com/cimgui/cimgui)
[ImGui-Beef](https://github.com/qzole/imgui-beef)

To build C/C++ API it needs the following parallel structure:
```
Root-Directory\
    |__ sdl2-beef\
    |__ bgfx-beef\    
    |__ imguibgfx-beef\
```

You need to clone [sdl2-beef](https://github.com/jazzbre/sdl2-beef) and [bgfx-beef](https://github.com/jazzbre/bgfx-beef) by yourself.

## Prerequisites
- To initialize submodules run *git submodule update --init --recursive*

## Windows
- Visual Studio 2019 Community/Professional (it can be built with other versions though, check build_windows_vs2019.cmd for more information)
- To build prerequisites run *build_windows_vs2019.cmd*

## MacOS
- To build prerequisites run *./build_macos.sh*

## Linux
- To build prerequisites run *./build_linux.sh* (EXPERIMENTAL - tested just that it builds and links on Ubuntu 20.04 LTS with WSL2)


## Usage
Open workspace and set Example as Startup project and Run!
MacOS/Linux can be built and run with provided VSCode project (expects Beef git clone in the same root folder as this project).

![](screenshot.png)

## Future work
iOS and Android build scripts.
