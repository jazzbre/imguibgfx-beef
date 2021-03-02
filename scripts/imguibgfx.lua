project "imguibgfx"
	kind "StaticLib"
	windowstargetplatformversion("10.0")

	includedirs {
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui"),
		path.join(SOURCE_DIR, "bx/include"),
		path.join(SOURCE_DIR, "../../sdl2-beef/submodules/SDL/include"),
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui/backends"),
		path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/include"),
		path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/examples/common/imgui"),
	}

	defines {
	}

	files {
		path.join(SOURCE_DIR, "imgui-beef/cimgui/cimgui.c*"),
		path.join(SOURCE_DIR, "imgui-beef/cimgui/cimgui.h*"),
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui/*.c*"),
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui/*.h*"),
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui/*.h*"),
		-- SDL2
		path.join(SOURCE_DIR, "imgui-beef/cimgui/imgui/backends/imgui_impl_sdl.*"),
		-- bgfx
		path.join(SOURCE_DIR, "../csrc/imguibgfx.cc"),
		path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/examples/common/imgui/*.h"),
	}

	configuration {}
	
   configuration { "linux* or vs20* or osx*" }
      defines { "IMGUIBGFX_VIEWPORTS" }
   
	configuration { "linux*" }
	defines {
		"SDL_VIDEO_DRIVER_X11"
	}
	
	configuration {}	
