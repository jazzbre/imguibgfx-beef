#include <vector>

#include <imgui.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h> 
#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui_impl_sdl.h>

#include "vs_imgui_texture.bin.h"
#include "fs_ocornut_imgui.bin.h"

// Constants
enum class BgfxTextureFlags : uint32_t {
	Opaque = 1u << 31,
	PointSampler = 1u << 30,
	All = Opaque | PointSampler,
};

// Shaders
static const bgfx::EmbeddedShader s_embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_imgui_texture),
	BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER_END()
};

// Helpers
static bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices) {
	return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout) && (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
}

// Native window functions

static void* GetNativeWindowHandle(SDL_Window* window) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window, &wmi)) {
		return NULL;
	}
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
	wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(window, "wl_egl_window");
	if (!win_impl) {
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		struct wl_surface* surface = wmi.info.wl.surface;
		if (!surface)
			return nullptr;
		win_impl = wl_egl_window_create(surface, width, height);
		SDL_SetWindowData(window, "wl_egl_window", win_impl);
	}
	return (void*)(uintptr_t)win_impl;
#else
	return (void*)wmi.info.x11.window;
#endif
#elif BX_PLATFORM_OSX
	return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
	return wmi.info.win.window;
#endif // BX_PLATFORM_
}

// Statics
static bgfx::VertexLayout  m_layout;
static bgfx::ProgramHandle m_program;
static bgfx::TextureHandle m_texture;
static bgfx::UniformHandle s_tex;
static ImFont* m_font;
static std::vector<bgfx::ViewId> freeViewIds;
static bgfx::ViewId viewIdIndex = 100;

// Structs

struct BgfxImGuiViewport {
	bgfx::FrameBufferHandle frameBufferHandle;
	bgfx::ViewId viewId = 0;
	uint16_t width = 0;
	uint16_t height = 0;
};

// Forward declarations
static void BgfxImGuiRender(bgfx::ViewId viewId, ImDrawData* draw_data, uint32_t clearColor);

// View Id functions

static bgfx::ViewId AllocateViewId() {
	if (freeViewIds.size()) {
		auto id = freeViewIds.back();
		freeViewIds.pop_back();
		return id;
	}
	return viewIdIndex++;
}

static void FreeViewId(bgfx::ViewId id) {
	freeViewIds.push_back(id);
}

// Window functions

static void ImguiBgfxOnCreateWindow(ImGuiViewport* viewport) {
	auto data = new BgfxImGuiViewport();
	viewport->RendererUserData = data;
	// Setup view id and size
	data->viewId = AllocateViewId();
	data->width = bx::max<uint16_t>((uint16_t)viewport->Size.x, 1);
	data->height = bx::max<uint16_t>((uint16_t)viewport->Size.y, 1);
	// Create frame buffer
	data->frameBufferHandle = bgfx::createFrameBuffer(GetNativeWindowHandle((SDL_Window*)viewport->PlatformHandle), data->width, data->height);
}

static void ImguiBgfxOnDestroyWindow(ImGuiViewport* viewport) {
	auto data = (BgfxImGuiViewport*)viewport->RendererUserData;
	if (!data) {
		return;
	}
	viewport->RendererUserData = nullptr;
	// Free id and destroy frame buffer
	FreeViewId(data->viewId);
	bgfx::destroy(data->frameBufferHandle);
	delete data;
}

static void ImguiBgfxOnSetWindowSize(ImGuiViewport* viewport, ImVec2 size) {
	auto data = (BgfxImGuiViewport*)viewport->RendererUserData;
	if (!data) {
		return;
	}
	const auto width = (uint16_t)size.x;
	const auto height = (uint16_t)size.y;
	// Check if resize is needed
	if (data->width == width && data->height == height) {
		return;
	}
	data->width = width;
	data->height = height;
	// Re-create frame buffer
	bgfx::destroy(data->frameBufferHandle);
	data->frameBufferHandle = bgfx::createFrameBuffer(GetNativeWindowHandle((SDL_Window*)viewport->PlatformHandle), data->width, data->height);
}

static void ImguiBgfxOnRenderWindow(ImGuiViewport* viewport, void*) {
	auto data = (BgfxImGuiViewport*)viewport->RendererUserData;
	if (!data) {
		return;
	}
	// Set frame buffer
	bgfx::setViewFrameBuffer(data->viewId, data->frameBufferHandle);
	// Render
	BgfxImGuiRender(data->viewId, viewport->DrawData, !(viewport->Flags & ImGuiViewportFlags_NoRendererClear) ? 0x000000ff : 0);
}

static void ImguiBgfxTryInitializeFonts() {
	static bool initialized = false;
	if (initialized) {
		return;
	}
	initialized = true;
	ImGuiIO& io = ImGui::GetIO();
	uint8_t* data;
	int32_t width;
	int32_t height;
	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	m_texture = bgfx::createTexture2D(
		(uint16_t)width
		, (uint16_t)height
		, false
		, 1
		, bgfx::TextureFormat::BGRA8
		, 0
		, bgfx::copy(data, width * height * 4)
	);
	io.Fonts->TexID = (ImTextureID)m_texture.idx;
}

// Exported functions

extern "C" bool ImguiBgfxInitialize(SDL_Window * window) {
	// Initialize ImGui
	ImGuiIO& io = ImGui::GetIO();
	// Enable docking and viewports
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
	// Setup render flags
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

	// Initialize SDL2
	if (!ImGui_ImplSDL2_InitForOpenGL(window, nullptr)) {
		return false;
	}

	// Setup window and render functions
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Renderer_CreateWindow = ImguiBgfxOnCreateWindow;
	platform_io.Renderer_DestroyWindow = ImguiBgfxOnDestroyWindow;
	platform_io.Renderer_SetWindowSize = ImguiBgfxOnSetWindowSize;
	platform_io.Renderer_RenderWindow = ImguiBgfxOnRenderWindow;

	// Initialize shaders
	const bgfx::RendererType::Enum type = bgfx::getRendererType();

	// Just a default texture shader
	m_program = bgfx::createProgram(
		bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_imgui_texture")
		, bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui")
		, true
	);

	m_layout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);
	return true;
}

extern "C" void ImguiBgfxFinalize() {
	ImGui::DestroyPlatformWindows();
	bgfx::destroy(s_tex);
	bgfx::destroy(m_texture);
	bgfx::destroy(m_program);
	ImGui_ImplSDL2_Shutdown();
}

static void BgfxImGuiRender(bgfx::ViewId viewId, ImDrawData* draw_data, uint32_t clearColor) {
	ImVec2 position = draw_data->DisplayPos;
	uint16_t width = (uint16_t)draw_data->DisplaySize.x;
	uint16_t height = (uint16_t)draw_data->DisplaySize.y;
	if (clearColor) {
		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clearColor, 1.0f, 0);
	}

	bgfx::setViewName(viewId, "ImGui");
	bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);

	bgfx::touch(viewId);

	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	float ortho[4][4] = { {2.0f / (R - L), 0.0f, 0.0f, 0.0f}, {0.0f, 2.0f / (T - B), 0.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.0f}, {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f} };
	bgfx::setViewTransform(viewId, nullptr, ortho);
	bgfx::setViewRect(viewId, 0, 0, uint16_t(width), uint16_t(height));

	// Render command lists
	for (int32_t ii = 0, num = draw_data->CmdListsCount; ii < num; ++ii) {
		bgfx::TransientVertexBuffer tvb;

		const ImDrawList* draw_list = draw_data->CmdLists[ii];
		uint32_t num_vertices = (uint32_t)draw_list->VtxBuffer.size();
		uint32_t num_indices = (uint32_t)draw_list->IdxBuffer.size();

		if (num_vertices == 0 || num_indices == 0) {
			continue;
		}

		if (!checkAvailTransientBuffers(num_vertices, m_layout, 0)) {
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		bgfx::allocTransientVertexBuffer(&tvb, num_vertices, m_layout);

		ImDrawVert* verts = (ImDrawVert*)tvb.data;
		bx::memCopy(verts, draw_list->VtxBuffer.begin(), num_vertices * sizeof(ImDrawVert));

		auto index_buffer_handle = bgfx::createIndexBuffer(bgfx::copy(draw_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx)));

		uint32_t offset = 0;
		for (const ImDrawCmd* cmd = draw_list->CmdBuffer.begin(), *cmdEnd = draw_list->CmdBuffer.end(); cmd != cmdEnd; ++cmd) {
			if (cmd->UserCallback) {
				cmd->UserCallback(draw_list, cmd);
			}
			else if (0 != cmd->ElemCount) {
				uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
				uint32_t sampler_state = 0;

				bgfx::TextureHandle th = m_texture;
				bgfx::ProgramHandle program = m_program;

				auto alphaBlend = true;
				if (cmd->TextureId != nullptr) {
					auto textureInfo = (uintptr_t)cmd->TextureId;
					if (textureInfo & (uint32_t)BgfxTextureFlags::Opaque) {
						alphaBlend = false;
					}
					if (textureInfo & (uint32_t)BgfxTextureFlags::PointSampler) {
						sampler_state = BGFX_SAMPLER_POINT;
					}
					textureInfo &= ~(uint32_t)BgfxTextureFlags::All;
					th = { (uint16_t)textureInfo };
				}
				if (alphaBlend) {
					state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				}
				const uint16_t xx0 = uint16_t(cmd->ClipRect.x - position.x);
				const uint16_t yy0 = uint16_t(cmd->ClipRect.y - position.y);
				const uint16_t xx1 = uint16_t(cmd->ClipRect.z - position.x);
				const uint16_t yy1 = uint16_t(cmd->ClipRect.w - position.y);
				bgfx::setScissor(xx0, yy0, xx1 - xx0, yy1 - yy0);

				bgfx::setState(state);
				bgfx::setTexture(0, s_tex, th, sampler_state);
				bgfx::setVertexBuffer(0, &tvb, 0, num_vertices);
				bgfx::setIndexBuffer(index_buffer_handle, offset, cmd->ElemCount);
				bgfx::submit(viewId, program);
			}

			offset += cmd->ElemCount;
		}

		bgfx::destroy(index_buffer_handle);
	}
}

extern "C" bool ImguiBgfxProcessEvent(const SDL_Event * event) {
	return ImGui_ImplSDL2_ProcessEvent(event);
}

extern "C" void ImguiBgfxNewFrame(SDL_Window * window) {
	ImguiBgfxTryInitializeFonts();
	ImGui_ImplSDL2_NewFrame(window);
}

extern "C" void ImguiBgfxRenderFrame(uint8_t viewId) {
	BgfxImGuiRender(viewId, ImGui::GetDrawData(), 0);
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
