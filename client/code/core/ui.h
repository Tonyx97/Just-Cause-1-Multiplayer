#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

class UI
{
private:

	struct download_progress_bar
	{
		mutable std::mutex mtx;

		std::string current_file;

		float smoothed_progress = 0.f,
			  progress = 0.f,
			  target = 0.f,
			  width = 600.f,
			  height = 30.f;

		bool enabled = false;

		void set_enabled(bool v)
		{
			std::lock_guard lock(mtx);
			enabled = v;

			if (!enabled)
				smoothed_progress = progress = target = 0.f;
		}

		void set_progress(float v)
		{
			std::lock_guard lock(mtx);
			progress = v;
		}

		void set_target(float v)
		{
			std::lock_guard lock(mtx);
			target = v;
		}

		void set_file(const std::string& v)
		{
			std::lock_guard lock(mtx);
			current_file = v;
		}

		float get_progress() const
		{
			std::lock_guard lock(mtx);
			return progress;
		}
	} download_bar {};

	HWND jc_hwnd = nullptr;

	bool initialized = false,
		 show_overlay_debug = true,
		 show_admin_panel = false;

	IDirect3DTexture9* splash_texture = nullptr;

	float splash_texture_alpha = 1.f;

	// ImGui bindings.
	//
#ifdef JC_DBG
	bool show_skeleton		 = true,
#else
	bool show_skeleton		 = false,
#endif
		 show_velocity		 = false,
		 show_distance		 = false,
		 show_health		 = false,
		 show_view_direction = false,
		 show_muzzle		 = false,
		 show_bullets		 = true,
		 show_grip			 = false,
		 show_last_muzzle	 = false,
		 show_last_grip		 = false,
		 infinite_ammo = false,
		 no_clip			 = true,
		 godmode			 = false,
		 show_top_dbg = false;

	int bullets_per_shoot = 1;

	ImGuiIO* io = nullptr;

	void begin();
	void render();
	void loading_screen();
	void render_players();
	void render_default_hud();
	void render_admin_panel();
	void render_download_bar();
	void overlay_debug();
	void net_debug();
	void end();

public:

	void init();
	void destroy();
	void dispatch();
	void toggle_admin_panel();
	void begin_window(const char* name, const ImVec2& pos, const ImVec2& size, const ImVec4& color);
	void end_window();
	void draw_filled_rect(float x, float y, float w, float h, const ImVec4& color);
	void draw_filled_circle(const vec2& center, float radius, int segs, const vec4& color);
	void draw_circle(const vec2& center, float radius, float thickness, int segs, const vec4& color);
	void draw_line(const vec2& p0, const vec2& p1, float thickness, const vec4& color);
	void draw_image(IDirect3DTexture9* texture, const vec2& pos, const vec2& size, const vec2& uv0 = { 0.f, 0.f }, const vec2& uv1 = { 1.f, 1.f }, const vec4& color = { 1.f, 1.f, 1.f, 1.f });

	float add_text(const char* text, float x, float y, float s, const ImVec4& color, bool center, float shadow = 0.f, float wrap = 0.f);
	float add_text(const wchar_t* text, float x, float y, float s, const ImVec4& color, bool center, float shadow = 0.f, float wrap = 0.f);

	float get_aspect_ratio() const { return io->DisplaySize.y / io->DisplaySize.x; }

	HWND get_window() const { return jc_hwnd; }

	ImVec2 calc_text_size(const char* text, float size, float wrap);
	ImVec2 get_screen_size() { return io->DisplaySize; }

	vec2 get_cursor_pos() const { return { io->MousePos.x, io->MousePos.y }; }

	// download bar methods

	void set_download_bar_progress(float v)				{ download_bar.set_progress(v); }
	void set_download_bar_enabled(bool v)				{ download_bar.set_enabled(v); }
	void set_download_bar_target(float v)				{ download_bar.set_target(v); }
	void set_download_bar_file(const std::string& v)	{ download_bar.set_file(v); }

	float get_download_bar_progress() const				{ return download_bar.get_progress(); }
};

inline UI* g_ui = nullptr;