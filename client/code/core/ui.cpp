#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"

#include <game/sys/all.h>

#include <game/transform/transform.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/camera/camera.h>
#include <game/object/weapon/bullet.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/agent_type/npc_variant.h>
#include <game/object/vars/exported_entities.h>
#include <game/object/vehicle/vehicle.h>

#include <shared_mp/objs/all.h>

#include <resource_sys/resource_system.h>

#include <mp/chat/chat.h>
#include <mp/net.h>

#define DEBUG_DRAW 1

void UI::init()
{
	// initialize ImGui interface and win api.

	jc_hwnd = FindWindow(L"Just Cause", nullptr);

	// should never happen, since our module lives in JC binary,
	// therefore window handle should always be in the list.
	// The only exception would be if someone would change the
	// default class name of the window.

	check(jc_hwnd, "Invalid game window");

	const auto dx_device = g_renderer->get_device();

	// rare case of race condition if render instance would be
	// cleaning up (or grabbed via gc). That would indicate the game
	// developers fucked up.

	check(dx_device, "Invalid dx9 device");

	ImGui::CreateContext();

	io = &ImGui::GetIO();

	// fuck light mode

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(jc_hwnd);
	ImGui_ImplDX9_Init(dx_device);

	const auto [font_data, font_size] = util::win::load_resource(GET_MODULE(), GAME_FONT, RT_FONT);

	io->Fonts->AddFontFromMemoryTTF(font_data, font_size, 20.f, nullptr, io->Fonts->GetGlyphRangesDefault());

	if (const auto splash_data = util::fs::read_bin_file(std::string(Net::DEFAULT_SERVER_FILES_PATH()) + "splash"); !splash_data.empty())
		D3DXCreateTextureFromFileInMemory(dx_device, splash_data.data(), splash_data.size(), &splash_texture);

	g_key->set_wnd_proc(jc_hwnd);

	SendMessageW(jc_hwnd, WM_SETICON, ICON_SMALL, GET_GAME_ICON());
	SendMessageW(jc_hwnd, WM_SETICON, ICON_BIG, GET_GAME_ICON());

	dbg = JC_ALLOC(DebugUI);

	initialized = true;

	log(GREEN, "Created UI");
}

void UI::destroy()
{
	if (!initialized)
		return;

	JC_FREE(dbg);

	// clean up initialization and win api proc handlers.

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	g_key->reset_wnd_proc(jc_hwnd);
	g_key->block_input(false);

	initialized = false;
}

void UI::dispatch()
{
	if (g_key->is_key_pressed(VK_F1))
		dbg->toggle_admin_panel();

	begin();

#if DEBUG_DRAW
	dbg->render_admin_panel();
#endif

	render();
	end();
}

void UI::set_cursor_pos(const vec2& v)
{
	io->WantSetMousePos = true;
	io->MousePos = ImVec2(v.x, v.y);
}

void UI::begin_window(const char* name, const ImVec2& pos, const ImVec2& size, const vec4& color)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ color.x, color.y, color.z, color.w });

	ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowPos(pos, ImGuiCond_Always);
	ImGui::SetWindowSize(size, ImGuiCond_Always);

	ImGui::PopStyleColor();
}

void UI::end_window()
{
	ImGui::End();
}

void UI::draw_line(const vec2& p0, const vec2& p1, float thickness, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddLine(ImVec2(p0.x, p0.y), ImVec2(p1.x, p1.y), ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), thickness);
}

void UI::draw_triangle(const vec2& p0, const vec2& p1, const vec2& p2, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddTriangle({ p0.x, p0.y }, { p1.x, p1.y }, { p2.x, p2.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

void UI::draw_filled_triangle(const vec2& p0, const vec2& p1, const vec2& p2, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddTriangleFilled({ p0.x, p0.y }, { p1.x, p1.y }, { p2.x, p2.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

void UI::draw_rect(const vec2& p, const vec2& size, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddQuad({ p.x, p.y }, { p.x, p.y + size.y }, { p.x + size.x, p.y + size.y }, { p.x + size.x, p.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

void UI::draw_filled_rect(const vec2& p, const vec2& size, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddQuadFilled({ p.x, p.y }, { p.x, p.y + size.y }, { p.x + size.x, p.y + size.y }, { p.x + size.x, p.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

void UI::draw_circle(const vec2& center, float radius, float thickness, int segs, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddCircle(ImVec2{ center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), segs, thickness);
}

void UI::draw_filled_circle(const vec2& center, float radius, int segs, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2 { center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), segs);
}

void UI::draw_image(IDirect3DTexture9* texture, const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddImage(texture, ImVec2 { pos.x, pos.y }, ImVec2 { pos.x + size.x, pos.y + size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

void UI::draw_text(const char* text, const vec2& p, float s, const vec4& color, bool center, float shadow, float wrap)
{
	auto dl = ImGui::GetWindowDrawList();

	const auto size = calc_text_size(text, s, wrap);

	const auto outline_color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)),
			   text_color = ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w });

	const auto position = (center ? ImVec2 { p.x - size.x / 2.f, p.y - size.y / 2.f } : ImVec2 { p.x, p.y });

	if (shadow > 0.f && shadow <= 3.14159f * 2.f)
	{
		float shadow_cos = std::cosf(shadow) * 2.f,
			  shadow_sin = std::sinf(shadow) * 2.f;

		dl->AddText(nullptr, s, ImVec2(position.x + shadow_cos, position.y + shadow_sin), outline_color, text, 0, wrap);
	}

	dl->AddText(nullptr, s, ImVec2(position.x, position.y), text_color, text, 0, wrap);
}

void UI::draw_text(const wchar_t* text, const vec2& p, float s, const vec4& color, bool center, float shadow, float wrap)
{
	auto str_length = std::wcslen(text) * 2;
	auto utf8_text = std::unique_ptr<char, std::function<void(char*)>>(new char[str_length](), [](char* data) { delete[] data; });

	if (WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8_text.get(), str_length, nullptr, nullptr) == 0)
		return;

	return draw_text(utf8_text.get(), p, s, color, center, shadow, wrap);
}

vec2 UI::calc_text_size(const char* text, float size, float wrap)
{
	const auto res = ImGui::GetWindowDrawList()->_Data->Font->CalcTextSizeA(size, FLT_MAX, wrap, text);

	return vec2(res.x, res.y);
}

void UI::begin()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void UI::render()
{
	const auto camera = g_camera->get_main_camera();

	static bool open_overlay = true;

	ImGui::SetNextWindowPos({ 0.f, 0.f }, ImGuiCond_Always);
	ImGui::SetNextWindowSize(io->DisplaySize, ImGuiCond_Always);

	constexpr auto window_flags = ImGuiWindowFlags_NoBackground |
								  ImGuiWindowFlags_NoCollapse |
								  ImGuiWindowFlags_NoDecoration |
								  ImGuiWindowFlags_NoResize |
								  ImGuiWindowFlags_NoTitleBar |
								  ImGuiWindowFlags_NoSavedSettings |
								  ImGuiWindowFlags_NoFocusOnAppearing |
								  ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowBgAlpha(0.f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });

	if (ImGui::Begin("overlay", &open_overlay, window_flags))
	{
		if (g_net->is_joined() && splash_texture_alpha <= 0.1f)
		{
			// always have the net stats rendering feature to
			// check net usage etc
			
			dbg->render_net();

#if DEBUG_DRAW
			dbg->render_overlay();
#endif

			render_players();
			render_default_hud();

			g_rsrc->trigger_event(script::event::ON_RENDER);
		}
		else loading_screen();

		// render the download bar if we are downloading something

		if (download_bar.enabled)
			render_download_bar();

		// lastly, update the chat

		g_chat->update();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

void UI::loading_screen()
{
	// if server provided splash texture then use it
	
	if (splash_texture)
	{
		ImGui::Image(splash_texture, io->DisplaySize, { 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 1.f, 1.f, splash_texture_alpha });

		if (g_net->is_joined())
			splash_texture_alpha = std::lerp(splash_texture_alpha, 0.f, g_time->get_delta() * 5.f);
		else
		{
			draw_text("Welcome to JC:MP! Please wait until all resources are downloaded and the game loads",
				vec2(0.1f * io->DisplaySize.x, 0.45f * io->DisplaySize.y), 24.f, vec4(1.f), false, true, 300.f);
		}
	}
}

void UI::render_players()
{
	const auto main_cam = g_camera->get_main_camera();
	if (!main_cam)
		return;

	const auto localplayer = g_net->get_localplayer();
	if (!localplayer)
		return;

	const auto local_char = localplayer->get_character();
	if (!local_char)
		return;

	g_net->for_each_player([&](NID, Player* player)
	{
		if (player->is_local() || !player->is_spawned())
			return true;

		// if tag is disabled then we won't render the player name or health
		
		if (!player->is_tag_enabled())
			return true;

		const auto player_char = player->get_character();
		const auto player_pos = player_char->get_position();

		constexpr float MAX_DISTANCE = 1000.f;

		if (auto distance_to_player = glm::distance(local_char->get_position(), player_pos); distance_to_player > 0.f && distance_to_player < MAX_DISTANCE)
		{
			const auto head_pos = player_char->get_bone_position(Head);

			if (vec2 out; main_cam->w2s(head_pos + vec3(0.f, 0.25f, 0.f), out))
			{
				const float name_size_adjust = 1.f - (distance_to_player / MAX_DISTANCE),
							hp_bar_size_adjust = 1.f - (distance_to_player / MAX_DISTANCE),
							hp = (player->get_hp() / player->get_max_hp()) * hp_bar_size_adjust,
							hp_bar_size_x = 100.f * hp_bar_size_adjust,
							hp_border_size = 2.f;

				draw_filled_rect(
					vec2(out.x - hp_border_size - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
					out.y - 2.f - 2.5f * hp_bar_size_adjust),
					vec2(hp_bar_size_x + hp_border_size * 2.f,
					2.5f * hp_bar_size_adjust + hp_border_size * 2.f),
					{ 0.f, 0.f, 0.f, 1.f });

				// don't try to draw this bar if the health is 0 lmao

				if (hp > 0.f)
				{
					draw_filled_rect(
						vec2(out.x - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
						out.y - 2.5f * hp_bar_size_adjust),
						vec2(hp * 100.f,
						2.5f * hp_bar_size_adjust),
						{ hp_bar_size_adjust - hp, hp, 0.f, 1.f });
				}

				draw_text(player->get_nick().c_str(), vec2(out.x, out.y - 10.f - 18.f * name_size_adjust), 18.f * name_size_adjust, { 1.f, 1.f, 1.f, 1.f }, true);
			}
		}

		return true;
	});
}

void UI::render_default_hud()
{
	const auto localplayer = g_net->get_localplayer();
	if (!localplayer)
		return;

	const auto local_char = localplayer->get_character();
	if (!local_char)
		return;

	const auto& [sx, sy] = io->DisplaySize;

	// render health bar
	
	const auto hp = local_char->get_real_hp(),
			   max_hp = local_char->get_max_hp();

	const auto normalized_hp = hp / max_hp;

	constexpr float bar_border = 4.f;

	const auto bar_pos = vec2(20.f, 15.f);
	const auto bar_size = vec2(250.f, 15.f);

	draw_filled_rect(bar_pos, bar_size + vec2(bar_border * 2.f, 0.f), { 0.f, 0.f, 0.f, 1.f });
	draw_filled_rect(bar_pos + vec2(bar_border, bar_border), bar_size - vec2(0.f, bar_border * 2.f), { 0.25f, 0.f, 0.f, 1.f });

	if (hp > 0.f)
		draw_filled_rect(bar_pos + vec2(bar_border), vec2(bar_size.x * normalized_hp, bar_size.y - bar_border * 2.f), { 1.f, 0.f, 0.f, 1.f });

	// render current weapon

	const auto weapons_sprite_item = g_texture_system->get_cache_item("new_equip_weapons_1");
	const auto grenade_icon_item = g_texture_system->get_cache_item("hud_grenade_icon");

	if (weapons_sprite_item && grenade_icon_item)
	{
		const auto weapons_sprite = weapons_sprite_item->get_texture();
		const auto grenade_icon = grenade_icon_item->get_texture();

		if (const auto belt = local_char->get_weapon_belt())
		{
			const auto curr_slot = belt->get_current_weapon_slot_id();
			const auto draw_slot = belt->get_draw_weapon_slot_id();

			{
				// render grenades (a line of grenades if we have less than 9)
				// and a simply multiplier when we have more

				const auto width = 30.f;
				const auto size = vec2(width, width * get_aspect_ratio() * 1.5f);
				const auto item_pos = vec2(25.f, 50.f);

				if (const auto grenades_count = local_char->get_grenades_ammo(); grenades_count <= 8)
				{
					for (int i = 0; i < 8; ++i)
					{
						const bool grenade_available = (i + 1) <= grenades_count;

						draw_image(grenade_icon, item_pos + vec2(i * width + 2.f, 2.f), size, vec2(0.f), vec2(1.f), vec4(0.f, 0.f, 0.f, 1.f));
						draw_image(grenade_icon, item_pos + vec2(i * width, 0.f), size, vec2(0.f), vec2(1.f), grenade_available ? vec4(1.f) : vec4(0.3f, 0.3f, 0.3f, 1.f));
					}
				}
				else
				{
					draw_image(grenade_icon, item_pos + vec2(2.f), size, vec2(0.f), vec2(1.f), vec4(0.f, 0.f, 0.f, 1.f));
					draw_image(grenade_icon, item_pos, size, vec2(0.f), vec2(1.f), vec4(1.f));

					draw_text(FORMATV("x {}", grenades_count).c_str(), vec2(25.f + 40.f, 50.f), 26.f, vec4(1.f), false, jc::nums::QUARTER_PI);
				}
			}

			// render current weapon icon and info
			
			if (const auto current_weapon = belt->get_weapon_from_slot(curr_slot))
			{
				const auto weapon_info = current_weapon->get_info();
				const auto [uv0, uv1] = weapon_info->get_icon_uvs();
				const auto color = curr_slot == draw_slot ? vec4(1.f) : vec4(0.5f, 0.5f, 0.5f, 1.f);
				const auto width = 150.f;
				const auto size = vec2(width, width * get_aspect_ratio() * 0.8f);
				const auto ammo = current_weapon->get_ammo();
				const auto total_ammo = belt->get_weapon_ammo(weapon_info->get_bullet_type());

				const auto item_pos = vec2(25.f, 100.f);

				draw_image(weapons_sprite, item_pos + vec2(2.f), size, uv0, uv1, { 0.f, 0.f, 0.f, 1.f });
				draw_image(weapons_sprite, item_pos, size, uv0, uv1, color);

				if (!weapon_info->has_infinite_ammo())
				{
					const auto separator_pos = item_pos + vec2(size.x + 40.f, size.y / 2.f);

					draw_text(FORMATV("{}", ammo).c_str(), separator_pos - vec2(0.f, 20.f), 26.f, vec4(1.f), true);
					draw_text(FORMATV("{}", total_ammo).c_str(), separator_pos + vec2(0.f, 20.f), 26.f, vec4(1.f), true);

					draw_line(separator_pos - vec2(20.f, 0.f), separator_pos + vec2(20.f, 0.f), 4.f, vec4(1.f));
				}
			}

			// render weapon radial wheel

			static std::unordered_map<WeaponType, float> item_angles;
			static WeaponType selected_type = WeaponType_Invalid;
			static float weapon_wheel_alpha = 0.f;
			static bool weapon_wheel_open = false;

			if (g_key->is_key_down(KEY_TAB))
			{
				const auto center = vec2(sx / 2.f, sy / 2.f);

				const auto item_distance_from_center = 250.f;
				const auto center_distance_to_inner = 175.f;
				const auto center_distance_to_outer = 325.f;

				draw_circle(center, item_distance_from_center, 150.f, 100, { 0.231f, 0.271f, 0.024f, 0.6f });	// main wheel
				draw_circle(center, center_distance_to_inner, 2.f, 100, { 0.f, 0.f, 0.f, 0.6f });				// inner border
				draw_circle(center, center_distance_to_outer, 2.f, 100, { 0.f, 0.f, 0.f, 0.6f });				// outer border

				const auto start_angle = jc::nums::PI * 3.f / 2.f;
				const auto end_angle = start_angle + jc::nums::DOUBLE_PI;
				const auto step_angle = jc::nums::DOUBLE_PI / static_cast<float>(WeaponType_Max - 1);

				// initialize item angles lookup map
				
				if (item_angles.empty())
				{
					float angle = start_angle;

					g_weapon->for_each_weapon_type([&](WeaponType type)
					{
						if (type == WeaponType_Signature || type == WeaponType_Special)
							return;

						item_angles.insert({ type, angle });

						angle += step_angle;
					});
				}

				float curr_angle = start_angle;

				g_weapon->for_each_weapon_type([&](WeaponType type)
				{
					if (type == WeaponType_Signature || type == WeaponType_Special)
						return;

					vec2 uv0, uv1;
					
					const auto weapon = belt->get_weapon_from_type(type);

					if (weapon)
						std::tie(uv0, uv1) = weapon->get_info()->get_icon_uvs();
					else std::tie(uv0, uv1) = WeaponInfo::CALCULATE_ICON_UVS(WeaponInfo::HAND_ICON());

					if (weapon || type == WeaponType_None)
					{
						float prev_angle = curr_angle - step_angle / 2.f,
							  next_angle = curr_angle + step_angle / 2.f;

						const auto angle_begin = vec2(std::cos(prev_angle), std::sin(prev_angle));
						const auto angle_end = vec2(std::cos(next_angle), std::sin(next_angle));
						const auto inner1 = center + angle_begin * center_distance_to_inner;
						const auto inner2 = center + angle_end * center_distance_to_inner;
						const auto outer1 = center + angle_begin * center_distance_to_outer;
						const auto outer2 = center + angle_end * center_distance_to_outer;

						draw_line(inner1, outer1, 2.f, { 0.f, 0.f, 0.f, weapon_wheel_alpha });
						draw_line(inner2, outer2, 2.f, { 0.f, 0.f, 0.f, weapon_wheel_alpha });

						const bool hovered = selected_type == type;
						
						const auto color = hovered ? vec4(0.8f, 0.7f, 0.2f, weapon_wheel_alpha) : vec4(0.3f, 0.3f, 0.3f, weapon_wheel_alpha);
						const auto item_center_pos = center + vec2(std::cos(curr_angle), std::sin(curr_angle)) * item_distance_from_center;

						if (hovered)
							draw_circle(item_center_pos, 52.f, 5.f, 30, color);	// weapon selection circle

						draw_filled_circle(item_center_pos, 50.f, 30, vec4(0.5f, 0.5f, 0.5f, weapon_wheel_alpha * 0.5f));	// weapon circle

						const auto width = 75.f;
						const auto size = vec2(width, width * get_aspect_ratio() * 0.8f);
						const auto item_pos = item_center_pos - vec2(size.x / 2.f, size.y / 2.f);

						draw_image(weapons_sprite, item_pos + vec2(2.f), size, uv0, uv1, { 0.f, 0.f, 0.f, weapon_wheel_alpha });
						draw_image(weapons_sprite, item_pos, size, uv0, uv1, { 1.f, 1.f, 1.f, weapon_wheel_alpha });

						std::string weapon_name;

						if (weapon)
						{
							const auto weapon_info = weapon->get_info();
							const auto ammo = weapon->get_ammo();
							const auto total_ammo = belt->get_weapon_ammo(weapon_info->get_bullet_type());

							if (hovered)
								weapon_name = weapon_info->get_name()->c_str();

							std::string text;

							if (weapon_info->has_infinite_ammo())
								text = "Infinite";
							else text = FORMATV("{} / {}", ammo, total_ammo);

							draw_text(text.c_str(), item_center_pos + vec2(0.f, 30.f), 16.f, { 1.f, 1.f, 1.f, weapon_wheel_alpha }, true, jc::nums::QUARTER_PI);
						}

						if (!weapon_name.empty())
							draw_text(weapon_name.c_str(), center - vec2(0.f, 30.f), 22.f, { 1.f, 1.f, 1.f, weapon_wheel_alpha }, true, jc::nums::QUARTER_PI);
					}

					curr_angle += step_angle;
				});

				if (!weapon_wheel_open)
				{
					set_cursor_pos(center);

					g_key->block_input(weapon_wheel_open = true);
				}
				else
				{
					const auto curr_cursor_pos = get_cursor_pos();
					const auto delta = curr_cursor_pos - center;
					const auto norm_delta = glm::normalize(delta);

					selected_type = WeaponType_Invalid;

					for (const auto& [type, angle] : item_angles)
					{
						const auto diff = std::fabsf(jc::math::delta_angle(std::atan2(norm_delta.y, norm_delta.x), angle));

						if (diff < step_angle / 2.f)
						{
							selected_type = type;
							break;
						}
					}

					// for better control, let's clamp the max cursor radius to "center_distance_to_inner" units
					// so in case the player moved the cursor very far, they don't need to
					// move it equally far to the other side to choose other weapons (sexy fix hehe)
					
					if (glm::length(delta) > center_distance_to_inner)
						set_cursor_pos(center + vec2(norm_delta.x, norm_delta.y) * center_distance_to_inner);

					if (weapon_wheel_alpha < 1.f)
						weapon_wheel_alpha += g_time->get_delta() * 5.f;
				}
			}
			else if (weapon_wheel_alpha > 0.f)
			{
				// when we stop holding the key, we will apply the changes no matter what
				
				if (weapon_wheel_open)
				{
					if (selected_type != WeaponType_Invalid && selected_type != WeaponType_None)
					{
						local_char->set_draw_weapon(belt->get_weapon_slot(selected_type));
						local_char->draw_weapon_now();
					}
					else local_char->hide_current_weapon();

					selected_type = WeaponType_Invalid;

					g_key->block_input(weapon_wheel_open = false);
				}

				weapon_wheel_alpha -= g_time->get_delta() * 5.f;
			}
		}
	}

	// render speedometer

	if (auto veh = local_char->get_vehicle(); veh && veh->get_driver_seat()->get_character() == local_char)
	{
		if (auto vehicle_net = g_net->get_net_object_by_game_object(veh)->cast<VehicleNetObject>())
		{
			if (g_key->is_key_pressed(KEY_X)) // todojc - debug shit
				vehicle_net->set_engine_state(!vehicle_net->get_engine_state(), true);

			vec2 center = vec2(sx - 250.f, sy - 50.f);

			const auto counters = 13;
			const auto start_angle = -jc::nums::PI;
			const auto step_angle = jc::nums::PI / static_cast<float>(counters - 1);
			const auto needle_length = 150.f;

			float angle = start_angle;

			for (int i = 0; i < counters; ++i)
			{
				const auto stage_pos = center + vec2(std::cos(angle), std::sin(angle)) * (needle_length + 10.f);

				draw_text(FORMATV("{}", float(i * 20.f)).c_str(), stage_pos, 20.f, vec4(1.f), true, jc::nums::QUARTER_PI);

				angle += step_angle;
			}

			static float smooth_velocity = 0.f;

			const auto velocity = (glm::length(veh->get_velocity()) * 3.6f) * (jc::nums::PI / 240.f);

			smooth_velocity = std::lerp(smooth_velocity, velocity, g_time->get_delta() * 10.f);

			const auto curr_velocity_pos = center + vec2(-std::cos(smooth_velocity), -std::sin(smooth_velocity)) * needle_length;

			draw_line(center, curr_velocity_pos, 2.f, vec4(1.f));

			draw_text(FORMATV("{:.1f} km/h", (glm::length(veh->get_velocity()) * 3.6f)).c_str(), center + vec2(0.f, 30.f), 22.f, vec4(1.f), true, jc::nums::QUARTER_PI);
		}
	}
}

void UI::render_download_bar()
{
	std::lock_guard lock(download_bar.mtx);

	if (download_bar.target > 0.f)
	{
		const auto& [sx, sy] = io->DisplaySize;

		// make the bar smooth
		
		download_bar.smoothed_progress = std::lerp(download_bar.smoothed_progress, download_bar.progress, 0.01f);

		const float progress = download_bar.smoothed_progress / download_bar.target;

		const auto bar_pos = vec2(sx / 2.f - download_bar.width / 2.f, sy - 100.f);

		draw_filled_rect(bar_pos, vec2(download_bar.width, download_bar.height), { 0.f, 0.f, 0.f, 0.5f });
		draw_filled_rect(bar_pos + vec2(2.f), vec2(progress * (download_bar.width - 4.f), download_bar.height - 4.f), { 0.222f, 0.476f, 0.351f, 1.f });

		draw_text(FORMATV("Downloading '{}'... {:.0f} % ({:.1f} / {:.1f} MB)",
			download_bar.current_file,
			progress * 100.f,
			download_bar.smoothed_progress / (1024.f * 1024.f),
			download_bar.target / (1024.f * 1024.f)).c_str(), vec2(sx / 2.f, bar_pos.y + 13.f), 18.f, vec4(1.f), true, 0);
	}
}

void UI::end()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}