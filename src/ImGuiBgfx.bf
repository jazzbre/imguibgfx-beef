using System;
using SDL2;

namespace imgui_beef
{
	static extension ImGui
	{
		[LinkName("ImguiBgfxInitialize")]
		public static extern bool BgfxInitialize(SDL.Window* window);

		[LinkName("ImguiBgfxFinalize")]
		public static extern void BgfxFinalize();

		[LinkName("ImguiBgfxProcessEvent")]
		public static extern bool BgfxProcessEvent(SDL.Event* event);

		[LinkName("ImguiBgfxNewFrame")]
		public static extern void BgfxNewFrame(SDL.Window* window);

		[LinkName("ImguiBgfxRenderFrame")]
		public static extern void BgfxRenderFrame(uint8 viewId);
	}
}
