using System;
using SDL2;

namespace imgui_beef
{
	extension ImGui
	{
		public enum BgfxTextureFlags : uint32 {
			Opaque = 1 << 31,
			PointSampler = 1 << 30,
		}

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
		
		public static void* BgfxGetTextureId(uint16 textureHandle, BgfxTextureFlags flags = 0) {
			var textureInfo = (uint32)textureHandle | (uint32)flags;
			return (void*)(uint)textureInfo;
		}

		[LinkName("ImguiBeginDockSpace")]
		public static extern void BeginDockSpace(ImGuiID dockspaceId, char8* windowName, ImGui.WindowFlags windowFlags = 0); // NOTE: after this menu can be setup and ImGui.End needs to be called
	}
}
