project "imguibgfx"
	kind "StaticLib"
   windowstargetplatformversion("10.0")

   includedirs {
      path.join(SOURCE_DIR, "cimgui/imgui"),
      path.join(SOURCE_DIR, "bx/include"),
      path.join(SOURCE_DIR, "../../sdl2-beef/submodules/SDL/include"),
      path.join(SOURCE_DIR, "cimgui/imgui/examples"),
      path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/include"),
      path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/examples/common/imgui"),
   }

   defines {
   }

	files {
      path.join(SOURCE_DIR, "cimgui/cimgui.c*"),
      path.join(SOURCE_DIR, "cimgui/cimgui.h*"),
      path.join(SOURCE_DIR, "cimgui/imgui/*.c*"),
      path.join(SOURCE_DIR, "cimgui/imgui/*.h*"),
      path.join(SOURCE_DIR, "cimgui/imgui/*.h*"),
      -- SDL2
      path.join(SOURCE_DIR, "cimgui/imgui/examples/imgui_impl_sdl.*"),
      -- bgfx
      path.join(SOURCE_DIR, "../csrc/imguibgfx.cc"),
      path.join(SOURCE_DIR, "../../bgfx-beef/submodules/bgfx/examples/common/imgui/*.h"),
	}

	configuration {}
