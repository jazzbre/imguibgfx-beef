# [imguibgfx-beef](https://github.com/jazzbre/imguibgfx-beef) ImGui wrapper (with built-in Bgfx/SDL2 support) for the Beef Programming Language

[Dear ImGui](https://github.com/ocornut/imgui)

## Dependencies
[cimgui](https://github.com/cimgui/cimgui)
[ImGui-Beef](https://github.com/qzole/imgui-beef)

To build C/C++ API it needs the following parallel structure:
Root-Directory\
    |__ imguibgfx-beef\    (this repository)
    |__ bgfx-beef\         (https://github.com/jazzbre/bgfx-beef.git)
    |__ SDL2\              (any mirror or unpacked zip will do)

## Prerequisites
- Visual Studio 2019 Community/Professional (it can be built with other versions though, check build_windows_vs2019.cmd for more information)
- To initialize submodules run *git submodule init*  and *git submodule update*
- To build prerequisites run *build_windows_vs2019.cmd*

## Usage

Open workspace and set Example as Startup project and Run!

## Future work
iOS, Android, macOS and Linux build scripts.
