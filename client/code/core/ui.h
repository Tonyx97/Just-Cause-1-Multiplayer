#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

class UI
{
private:
	HWND jc_hwnd = nullptr;

	std::once_flag initialize_flag;

	bool initialized = false,
		 show_debug	 = false;

	// ImGui bindings.
	//
	bool show_skeleton		 = true,
		 show_velocity		 = false,
		 show_distance		 = true,
		 show_health		 = false,
		 show_view_direction = false,
		 show_muzzle		 = true,
		 show_bullets		 = true,
		 show_grip			 = true,
		 show_last_muzzle	 = false,
		 show_last_grip		 = false,
		 no_clip			 = true,
		 spawn_vtable_hooked = false;

	int bullets_per_shoot = 1;

	std::atomic_bool destroying = false,
					 destroyed	= false;

	ImGuiIO* io = nullptr;

	void begin();
	void render();
	void build_debug();
	void end();

public:
	void init();
	void destroy();
	void dispatch();
	void begin_window(const char* name, const ImVec2& pos, const ImVec2& size, const ImVec4& color);
	void end_window();
	void draw_filled_rect(float x, float y, float w, float h, const ImVec4& color);

	void toggle_debug()
	{
		show_debug = !show_debug;
	}

	bool is_destroying() const { return destroying; }
	bool is_destroyed() const { return destroyed; }

	float add_text(const char* text, float x, float y, float s, const ImVec4& color, bool center, int shadow = -1, float wrap = 0.f);
	float add_text(const wchar_t* text, float x, float y, float s, const ImVec4& color, bool center, int shadow = -1, float wrap = 0.f);

	ImVec2 calc_text_size(const char* text, float size, float wrap);
	ImVec2 get_screen_size() { return io->DisplaySize; }
};

inline UI* g_ui = nullptr;