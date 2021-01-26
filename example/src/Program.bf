using System;
using SDL2;
using Bgfx;
using ImGui;

namespace example
{
	class Program
	{
		private SDL.Window* window;
		private ImGui.Context* context;
		int32 width = 640;
		int32 height = 480;

		private int Test()
		{
			// Initialize SDL
			SDL.Init(.Video | .Events);
			// Create window
			window = SDL.CreateWindow("ImGuiBgfx Example", .Undefined, .Undefined, width, height, .Shown | .Resizable);
			SDL.MaximizeWindow(window);

			// Initialize bgfx
			var info = SDL.SDL_SysWMinfo();
			SDL.GetWindowWMInfo(window, ref info);

			var platformData = bgfx.PlatformData();
			platformData.ndt = null;
#if BF_PLATFORM_WINDOWS
			platformData.nwh = (void*)(int)info.info.win.window;
			#elif BF_PLATFORM_MACOS
						platformData.nwh = (void*)(int)info.info.cocoa.window;
			#elif BF_PLATFORM_LINUX
						platformData.nwh = (void*)(int)info.info.x11.window;
#endif

			bgfx.render_frame(0);
			var init = bgfx.Init();
			init.platformData = platformData;
			init.type = bgfx.RendererType.Direct3D11;
			init.resolution.format = bgfx.TextureFormat.RGBA8;
			init.resolution.width = (uint32)width;
			init.resolution.height = (uint32)height;
			init.resolution.reset = (uint32)bgfx.ResetFlags.Vsync;
			init.resolution.numBackBuffers = 2;
			init.limits.maxEncoders = 8;
			init.limits.minResourceCbSize = 65536;
			init.limits.transientVbSize = 6291456;
			init.limits.transientIbSize = 2097152;
			bgfx.init(&init);

			// Initialize ImGui
			context = ImGui.CreateContext();
			ImGui.StyleColorsDark();

			ImGui.GetIO().ConfigFlags |= .NavEnableKeyboard;// Enable Keyboard Navigation Controls

			if (!ImGui.BgfxInitialize(window))
			{
				Console.WriteLine("Failed to initialize ImGuiBgfx!");
				return 0;
			}

			// Main loop!
			var quitting = false;
			while (!quitting)
			{
				SDL.Event event;
				while (SDL.PollEvent(out event) != 0)
				{
					ImGui.BgfxProcessEvent(&event);
					switch (event.type)
					{
					case .Quit:
						quitting = true;
					case .WindowEvent:
						switch (event.window.windowEvent) {
						case .Resized:
							if (event.window.windowID == SDL.GetWindowID(window))
							{
								width = event.window.data1;
								height = event.window.data2;
								bgfx.reset((uint32)width, (uint32)height, (uint32)bgfx.ResetFlags.Vsync, Bgfx.bgfx.TextureFormat.Count);
								Console.WriteLine("Resized {0}x{0}", width, height);
							}
						default:
						}
					default:
					}
				}
				// Clear
				uint8 viewId = 0;
				bgfx.set_view_clear(viewId, (uint)(bgfx.ClearFlags.Color | bgfx.ClearFlags.Depth), 0x2255ffff, 1.0f, 0);
				bgfx.set_view_rect(viewId, 0, 0, (uint16)width, (uint16)height);
				bgfx.touch(viewId);

				// Render some bgfx stuff or not...

				// Render ImGui
				ImGui.BgfxNewFrame(window);
				ImGui.NewFrame();
				ImGui.ShowDemoWindow();
				ImGui.Render();
				ImGui.BgfxRenderFrame(99);

				// End frame bgfx
				bgfx.frame(false);
			}

			// Shutdown ImGui
			ImGui.BgfxFinalize();
			ImGui.DestroyContext(context);

			// Shutdown bgfx
			bgfx.shutdown();

			// Destroy window
			SDL.DestroyWindow(window);

			return 0;
		}

		static int Main()
		{
			var program = scope Program();
			return program.Test();
		}
	}
}
