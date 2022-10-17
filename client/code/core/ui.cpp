#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"
#include "test_units.h"

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

#include <mp/chat/chat.h>
#include <mp/net.h>

uintptr_t original_functions[43];
uintptr_t return_address;
uintptr_t first_argument;
uintptr_t second_argument;
uintptr_t third_argument;
uintptr_t fourth_argument;
uintptr_t original_function;
uint32_t  function_index;
void __declspec(naked) neutral_hook2(void)
{
	__asm {
		push 0xDEADBEEF
		push ebp
		mov ebp, esp
		push eax
		push ecx
		push edx

		mov first_argument, ecx
			// mov second_argument, edx

		mov eax, [ebp + 0x8]
		mov return_address, eax
		mov eax, [ebp + 0xC]
		mov second_argument, eax
		mov eax, [ebp + 0x10]
		mov third_argument, eax
		mov eax, [ebp + 0x14]
		mov fourth_argument, eax

		mov eax, [ebp+8]
		mov eax, [eax-4]
		mov ecx, eax
		shr eax, 2
		shr ecx, 18h
		cmp eax, 2Bh
		jbe done
		mov eax, ecx
		xor ecx, ecx
		shr eax, 2
		cmp eax, 2Bh
		cmova eax, ecx

		done:
		mov function_index, eax
		mov eax, original_functions[eax*4]
		mov original_function, eax
		mov [esp + 0x10], eax
	}

	log(GREEN, "Called from: {:x} (index: {} at: {:x})\n\targ1: {:x}\n\targ2: {:x} ({:.1f})\n\targ3: {:x} ({:.1f})\n\targ4: {:x} ({:.1f})\n", return_address, function_index, original_function, first_argument, second_argument, (float)second_argument, third_argument, (float)third_argument, fourth_argument, float(fourth_argument));

	__asm {
		pop edx
		pop ecx
		pop eax
		pop ebp

		ret
	}
}

float image_x = 1.f,
	  image_y = 1.f;

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

	initialized = true;

	log(GREEN, "Created UI");
}

void UI::destroy()
{
	if (!initialized)
		return;

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
		toggle_admin_panel();

	begin();
	render();
	end();
}

void UI::toggle_admin_panel()
{
	show_admin_panel = !show_admin_panel;

	io->MouseDrawCursor = show_admin_panel;

	g_key->block_input(show_admin_panel);
}

void UI::begin_window(const char* name, const ImVec2& pos, const ImVec2& size, const ImVec4& color)
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, color);

	ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowPos(pos, ImGuiCond_Always);
	ImGui::SetWindowSize(size, ImGuiCond_Always);

	ImGui::PopStyleColor();
}

void UI::end_window()
{
	ImGui::End();
}

void UI::draw_filled_rect(float x, float y, float w, float h, const ImVec4& color)
{
	ImGui::GetWindowDrawList()->AddQuadFilled({ x, y }, { x, y + h }, { x + w, y + h }, { x + w, y }, ImGui::ColorConvertFloat4ToU32(color));
}

void UI::draw_filled_circle(const vec2& center, float radius, int segs, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2 { center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), segs);
}

void UI::draw_circle(const vec2& center, float radius, float thickness, int segs, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddCircle(ImVec2{ center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), segs, thickness);
}

void UI::draw_line(const vec2& p0, const vec2& p1, float thickness, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddLine(ImVec2(p0.x, p0.y), ImVec2(p1.x, p1.y), ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }), thickness);
}

void UI::draw_image(IDirect3DTexture9* texture, const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const vec4& color)
{
	ImGui::GetWindowDrawList()->AddImage(texture, ImVec2 { pos.x, pos.y }, ImVec2 { pos.x + size.x, pos.y + size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y }, ImGui::ColorConvertFloat4ToU32(ImVec4{ color.x, color.y, color.z, color.w }));
}

float UI::add_text(const char* text, float x, float y, float s, const ImVec4& color, bool center, float shadow, float wrap)
{
	auto dl = ImGui::GetWindowDrawList();

	const auto size = calc_text_size(text, s, wrap);

	const auto outline_color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)),
			   text_color = ImGui::ColorConvertFloat4ToU32(color);

	const auto position = (center ? ImVec2 { x - size.x / 2.f, y - size.y / 2.f } : ImVec2 { x, y });

	if (shadow > 0.f && shadow <= 3.14159f * 2.f)
	{
		float shadow_cos = std::cosf(shadow) * 2.f,
			  shadow_sin = std::sinf(shadow) * 2.f;

		dl->AddText(nullptr, s, ImVec2(position.x + shadow_cos, position.y + shadow_sin), outline_color, text, 0, wrap);
	}

	dl->AddText(nullptr, s, ImVec2(position.x, position.y), text_color, text, 0, wrap);

	return y + size.y;
}

float UI::add_text(const wchar_t* text, float x, float y, float s, const ImVec4& color, bool center, float shadow, float wrap)
{
	auto str_length = std::wcslen(text) * 2;
	auto utf8_text = std::unique_ptr<char, std::function<void(char*)>>(new char[str_length](), [](char* data) { delete[] data; });

	if (WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8_text.get(), str_length, nullptr, nullptr) == 0)
		return 0.f;

	return add_text(utf8_text.get(), x, y, s, color, center, shadow, wrap);
}

ImVec2 UI::calc_text_size(const char* text, float size, float wrap)
{
	return ImGui::GetWindowDrawList()->_Data->Font->CalcTextSizeA(size, FLT_MAX, wrap, text);
}

void UI::begin()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void UI::render()
{
	render_admin_panel();

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
			net_debug();

			if (show_overlay_debug)
				overlay_debug();

			render_players();
			render_default_hud();
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
			add_text("Welcome to JC:MP! Please wait until all resources are downloaded and the game loads",
				0.1f * io->DisplaySize.x, 0.45f * io->DisplaySize.y, 24.f, { 1.f, 1.f, 1.f, 1.f }, false, true, 300.f);
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
					out.x - hp_border_size - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
					out.y - 2.f - 2.5f * hp_bar_size_adjust,
					hp_bar_size_x + hp_border_size * 2.f,
					2.5f * hp_bar_size_adjust + hp_border_size * 2.f,
					{ 0.f, 0.f, 0.f, 1.f });

				// don't try to draw this bar if the health is 0 lmao

				if (hp > 0.f)
				{
					draw_filled_rect(
						out.x - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
						out.y - 2.5f * hp_bar_size_adjust,
						hp * 100.f,
						2.5f * hp_bar_size_adjust,
						{ hp_bar_size_adjust - hp, hp, 0.f, 1.f });
				}

				add_text(player->get_nick().c_str(), out.x, out.y - 10.f - 18.f * name_size_adjust, 18.f * name_size_adjust, { 1.f, 1.f, 1.f, 1.f }, true);
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

	constexpr float bar_x = 20.f;
	constexpr float bar_y = 15.f;
	constexpr float bar_width = 250.f;
	constexpr float bar_height = 15.f;
	constexpr float bar_border = 4.f;

	draw_filled_rect(bar_x, bar_y, bar_width + bar_border * 2.f, bar_height, { 0.f, 0.f, 0.f, 1.f });
	draw_filled_rect(bar_x + bar_border, bar_y + bar_border, bar_width, bar_height - bar_border * 2.f, { 0.25f, 0.f, 0.f, 1.f });

	if (hp > 0.f)
		draw_filled_rect(bar_x + bar_border, bar_y + bar_border, bar_width * normalized_hp, bar_height - bar_border * 2.f, { 1.f, 0.f, 0.f, 1.f });

	// render current weapon

	const auto grenades_count = local_char->get_grenades_ammo();
	
	const auto list = g_texture_system->get_cache();

	if (const auto n = list->find("new_equip_weapons_1"))
	{
		const auto aa = *(void**)(ptr(n) + 0x28);	// todojc - debug, get it properly
		const auto texture = jc::read<IDirect3DTexture9*>(aa, 0x44);

		const auto n2 = list->find("hud_grenade_icon");
		const auto aa2 = *(void**)(ptr(n2) + 0x28);	// todojc - debug, get it properly
		const auto grenade_texture = jc::read<IDirect3DTexture9*>(aa2, 0x44);

		if (const auto belt = local_char->get_weapon_belt())
		{
			const auto curr_slot = belt->get_current_weapon_slot_id();
			const auto draw_slot = belt->get_draw_weapon_slot_id();

			{
				// render grenades (a line of grenades if we have less than 9)
				// and a simply multiplier when we have more

				const auto width = 30.f;
				const auto height = width * get_aspect_ratio() * 1.5f;

				if (grenades_count <= 8)
				{
					for (int i = 0; i < 8; ++i)
					{
						const bool grenade_available = (i + 1) <= grenades_count;

						draw_image(grenade_texture, vec2(25.f + i * width + 2.f, 50.f + 2.f), { width, height }, vec2(0.f), vec2(1.f), vec4(0.f, 0.f, 0.f, 1.f));
						draw_image(grenade_texture, vec2(25.f + i * width, 50.f), { width, height }, vec2(0.f), vec2(1.f), grenade_available ? vec4(1.f) : vec4(0.3f, 0.3f, 0.3f, 1.f));
					}
				}
				else
				{
					draw_image(grenade_texture, vec2(25.f + 2.f, 50.f + 2.f), { width, height }, vec2(0.f), vec2(1.f), vec4(0.f, 0.f, 0.f, 1.f));
					draw_image(grenade_texture, vec2(25.f, 50.f), { width, height }, vec2(0.f), vec2(1.f), vec4(1.f));

					add_text(FORMATV("x {}", grenades_count).c_str(), 25.f + 40.f, 50.f, 26.f, { 1.f, 1.f, 1.f, 1.f }, false, jc::nums::QUARTER_PI);
				}
			}

			if (const auto current_weapon = belt->get_weapon_from_slot(curr_slot))
			{
				const auto weapon_info = current_weapon->get_info();
				const auto [uv0, uv1] = weapon_info->get_icon_uvs();
				const auto color = curr_slot == draw_slot ? vec4(1.f, 1.f, 1.f, 1.f) : vec4(0.5f, 0.5f, 0.5f, 1.f);
				const auto width = 150.f;
				const auto height = width * get_aspect_ratio() * 0.8f;
				const auto ammo = current_weapon->get_ammo();
				const auto total_ammo = belt->get_weapon_ammo(weapon_info->get_bullet_type());

				const auto item_pos = vec2(25.f, 100.f);

				draw_image(texture, item_pos + vec2(2.f), { width, height }, uv0, uv1, { 0.f, 0.f, 0.f, 1.f });
				draw_image(texture, item_pos, { width, height }, uv0, uv1, color);

				if (!weapon_info->has_infinite_ammo())
				{
					const auto separator_pos = item_pos + vec2(width + 40.f, height / 2.f);

					add_text(FORMATV("{}", ammo).c_str(), separator_pos.x, separator_pos.y - 20.f, 26.f, { 1.f, 1.f, 1.f, 1.f }, true);
					add_text(FORMATV("{}", total_ammo).c_str(), separator_pos.x, separator_pos.y + 20.f, 26.f, { 1.f, 1.f, 1.f, 1.f }, true);

					draw_line(separator_pos - vec2(20.f, 0.f), separator_pos + vec2(20.f, 0.f), 4.f, vec4(1.f));
				}
			}

			// render weapon radial wheel

			static vec2 weapon_cursor_direction = {};
			static vec2 weapon_cursor_start_pos = {};
			static WeaponType selected_type = WeaponType_Invalid;
			static float weapon_wheel_alpha = 0.f;
			static bool weapon_wheel_open = false;

			if (g_key->is_key_down(KEY_TAB))
			{
				const auto center = vec2(sx / 2.f, sy / 2.f);

				const auto item_distance_from_center = 250.f;
				const auto center_distance_to_inner = 175.f;
				const auto center_distance_to_outer = 325.f;

				draw_circle(center, item_distance_from_center, 150.f, 100, { 0.2f, 0.3f, 0.3f, 0.6f });
				draw_circle(center, center_distance_to_inner, 2.f, 100, { 0.f, 0.f, 0.f, 0.6f });
				draw_circle(center, center_distance_to_outer, 2.f, 100, { 0.f, 0.f, 0.f, 0.6f });

				const auto start_angle = jc::nums::PI * 3.f / 2.f;
				const auto end_angle = start_angle + jc::nums::DOUBLE_PI;
				const auto step_angle = jc::nums::DOUBLE_PI / float(WeaponType_Max - 1);

				float curr_angle = start_angle;

				static std::unordered_map<WeaponType, float> type_angles;

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
						if (auto it = type_angles.find(type); it == type_angles.end())
							type_angles.insert({ type, curr_angle });

						const bool hovered = selected_type == type;

						const auto angle_begin = vec2(std::cos(curr_angle - step_angle / 2.f), std::sin(curr_angle - step_angle / 2.f));
						const auto angle_end = vec2(std::cos(curr_angle + step_angle / 2.f), std::sin(curr_angle + step_angle / 2.f));
						const auto line1_base = center + angle_begin * center_distance_to_inner;
						const auto line2_base = center + angle_end * center_distance_to_inner;
						const auto line1_target = center + angle_begin * center_distance_to_outer;
						const auto line2_target = center + angle_end * center_distance_to_outer;

						draw_line(line1_base, line1_target, 2.f, { 0.f, 0.f, 0.f, weapon_wheel_alpha });
						draw_line(line2_base, line2_target, 2.f, { 0.f, 0.f, 0.f, weapon_wheel_alpha });
						
						const auto color = hovered ? vec4(0.8f, 0.7f, 0.2f, weapon_wheel_alpha) : vec4(0.3f, 0.3f, 0.3f, weapon_wheel_alpha);
						const auto item_center_pos = center + vec2(std::cos(curr_angle), std::sin(curr_angle)) * item_distance_from_center;

						if (hovered)
							draw_circle(item_center_pos, 52.f, 5.f, 30, color);

						draw_filled_circle(item_center_pos, 50.f, 30, vec4(0.3f, 0.4f, 0.4f, weapon_wheel_alpha * 0.5f));

						const auto width = 75.f;
						const auto height = width * get_aspect_ratio() * 0.8f;

						const auto item_pos = item_center_pos - vec2(width / 2.f, height / 2.f);

						draw_image(texture, item_pos + vec2(2.f), { width, height }, uv0, uv1, { 0.f, 0.f, 0.f, weapon_wheel_alpha });
						draw_image(texture, item_pos, { width, height }, uv0, uv1, { 1.f, 1.f, 1.f, weapon_wheel_alpha });

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

							add_text(text.c_str(), item_center_pos.x, item_center_pos.y + 30.f, 16.f, { 1.f, 1.f, 1.f, weapon_wheel_alpha }, true, jc::nums::QUARTER_PI);
						}

						if (!weapon_name.empty())
							add_text(weapon_name.c_str(), center.x, center.y - 30.f, 22.f, { 1.f, 1.f, 1.f, weapon_wheel_alpha }, true, jc::nums::QUARTER_PI);
					}

					curr_angle += step_angle;
				});

				if (!weapon_wheel_open)
				{
					weapon_cursor_start_pos = get_cursor_pos();
					
					g_key->block_input(weapon_wheel_open = true);
				}
				else
				{
					const auto curr_cursor_pos = get_cursor_pos();
					const auto delta = glm::normalize(curr_cursor_pos - weapon_cursor_start_pos);

					selected_type = WeaponType_Invalid;

					for (const auto& [type, angle] : type_angles)
					{
						const auto diff = std::fabsf(jc::math::delta_angle(std::atan2(delta.y, delta.x), angle));

						if (diff < step_angle / 2.f)
						{
							selected_type = type;
							break;
						}
					}

					if (weapon_wheel_alpha < 1.f)
						weapon_wheel_alpha += g_time->get_delta() * 5.f;
				}
			}
			else if (weapon_wheel_open)
			{
				if (weapon_wheel_alpha > 0.f)
					weapon_wheel_alpha -= g_time->get_delta() * 5.f;
				else
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
			}
		}
	}
}

void UI::render_admin_panel()
{
	if (g_chat->is_typing())
		return;

	const auto main_cam = g_camera->get_main_camera();
	if (!main_cam)
		return;

	const auto localplayer = g_net->get_localplayer();
	if (!localplayer)
		return;

	const auto local_char = localplayer->get_character();
	if (!local_char)
		return;

	// init stuff

	static WeaponTemplate* templeit = 0;

	if (!templeit)	// create new weapon :o
	{
		std::string model = "m488.rbm";

		object_base_map map{};
		map.insert<object_base_map::Int>(0x2419daa1, 15); // int
		map.insert<object_base_map::Int>(0x2caec4c6, 3); // int
		map.insert<object_base_map::Int>(0x2e3e2094, 1); // int
		map.insert<object_base_map::Int>(0x2ecc28f9, -1); // int
		map.insert<object_base_map::Int>(0x3c6afa74, -1); // int
		map.insert<object_base_map::Int>(0x67f4b92, 1); // int
		map.insert<object_base_map::Int>(0x73c279d9, 7); // int
		map.insert<object_base_map::Int>(0x7932fd00, 3); // int
		map.insert<object_base_map::Int>(0x92d1b4b0, 1); // int
		map.insert<object_base_map::Int>(0x983bdc19, 16); // int
		map.insert<object_base_map::Int>(0xb48c8992, 14); // int
		map.insert<object_base_map::Int>(0xc17ff89f, 100); // int
		map.insert<object_base_map::Int>(0xdc4e8e89, 12); // int
		map.insert<object_base_map::Int>(0xe43f6ff8, 0); // int
		map.insert<object_base_map::Int>(0xea9b2d84, 2); // int
		map.insert<object_base_map::Int>(0xebb4f93f, 1); // int
		map.insert<object_base_map::Int>(0xf776041b, 1); // int
		map.insert<object_base_map::Float>(0x2524eee2, 7.00f); // float
		map.insert<object_base_map::Float>(0x3ae7b611, 500.00f); // float - max radius
		map.insert<object_base_map::Float>(0x3c973815, 1.00f); // float
		map.insert<object_base_map::Float>(0x43d3fa78, 1.00f); // float
		map.insert<object_base_map::Float>(0x51905fda, 50.00f); // float
		map.insert<object_base_map::Float>(0x58dd9170, 50.00f); // float
		map.insert<object_base_map::Float>(0x61d710ac, 100.00f); // float
		map.insert<object_base_map::Float>(0x6e24e123, 1.50f); // float
		map.insert<object_base_map::Float>(0x72fdb359, 500.00f); // float
		map.insert<object_base_map::Float>(0x737cf1df, 1.25f); // float
		map.insert<object_base_map::Float>(0x83fcbe6d, 0.32f); // float
		map.insert<object_base_map::Float>(0x88bbee10, 1.20f); // float
		map.insert<object_base_map::Float>(0x8dccf8a, 200.00f); // float
		map.insert<object_base_map::Float>(0x9db0dcdf, 130.00f); // float
		map.insert<object_base_map::Float>(0xb27fbdf, 9000.00f); // float - ammo max range
		map.insert<object_base_map::Float>(0xc5a583e4, 7.00f); // float
		map.insert<object_base_map::Float>(0xc9dae566, 1.00f); // float
		map.insert<object_base_map::Float>(0xe775a5da, 2.00f); // float
		map.insert<object_base_map::Float>(0xe807fbbe, 0.10f); // float
		map.insert<object_base_map::Float>(0xf400ec2a, 1.00f); // float
		map.insert<object_base_map::Float>(0xfdc2fc1f, 60.00f); // float
		map.insert<object_base_map::Float>(0xfde6d38c, 0.01f); // float
		map.insert<object_base_map::String>(0xa9818615, R"(VFX_I_Muzzle_Rocket_Launcher)"); // string
		map.insert<object_base_map::String>(0xb231ec06, R"(M488)"); // string
		map.insert<object_base_map::String>(0xb3776904, R"(360_exp_4_1)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Rocket Launcher)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Model, model); // string

		templeit = g_weapon->create_weapon_template(&map);

		g_texture_system->load_texture("dummy_black.dds");
		g_model_system->load_rbm(model);

		log(RED, "{:x}", jc::game::hash_str("ammo_max_range"));

		/*g_texture_system->unload_texture("dummy_black.dds");
		g_model_system->unload_rbm(model);*/
	}

	// weapon stuff

	if (auto current_weapon = local_char->get_weapon_belt()->get_current_weapon())
	{
		current_weapon->get_info()->set_infinite_ammo(infinite_ammo);
	}

	if (!show_admin_panel)
		return;

	io->MouseDrawCursor = true;

	ImGui::SetNextWindowSize({ 700.f, 700.f }, ImGuiCond_Once);
	ImGui::Begin("Admin Panel");

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	if (g_key->is_key_pressed(KEY_F))
		local_char->play_idle_stance();

	if (ImGui::TreeNode("Server"))
	{
		float day_time = g_day_cycle->get_hour();
		float timescale = g_time->get_time_scale();
		float punch_force = Character::GET_GLOBAL_PUNCH_DAMAGE(false);

		if (ImGui::SliderFloat("Day Hour##ap.sv.time", &day_time, 0.f, 24.f))
			g_net->send(Packet(DbgPID_SetTime, ChannelID_Debug, day_time));

		if (ImGui::SliderFloat("Time Scale (lol, don't touch sir)##ap.sv.ts", &timescale, 0.01f, 5.f))
			g_net->send(Packet(WorldPID_SetTimeScale, ChannelID_World, timescale));

		if (ImGui::SliderFloat("Punch Force (lmao)##ap.sv.pf", &punch_force, 50.f, 10000.f))
			g_net->send(Packet(WorldPID_SetPunchForce, ChannelID_World, punch_force));

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("General"))
	{
		ImGui::Checkbox("No clip", &no_clip);

		if (ImGui::Checkbox("God mode", &godmode))
		{
			if (godmode)
				local_char->set_alive_flag(AliveObjectFlag_Invincible);
			else local_char->remove_alive_flag(AliveObjectFlag_Invincible);
		}

		ImGui::Checkbox("Show Top Debug", &show_top_dbg);

		if (ImGui::Button("Toggle world lighting"))
			g_day_cycle->set_night_time_enabled(!g_day_cycle->is_night_time_enabled());

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Characters"))
	{
		ImGui::Checkbox("Skeleton", &show_skeleton);
		ImGui::Checkbox("Velocity", &show_velocity);
		ImGui::Checkbox("Distance", &show_distance);
		ImGui::Checkbox("Health", &show_health);
		ImGui::Checkbox("View direction", &show_view_direction);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Weapons"))
	{
		ImGui::Checkbox("Bullets", &show_bullets);
		ImGui::Checkbox("Muzzle", &show_muzzle);
		ImGui::Checkbox("Last muzzle", &show_last_muzzle);
		ImGui::Checkbox("Grip", &show_grip);
		ImGui::Checkbox("Last grip", &show_last_grip);
		ImGui::Checkbox("Infinite Ammos", &infinite_ammo);

		static int weapon_to_give = 0;

		static WeaponID weapon_ids[] =
		{
			Weapon_Pistol,
			Weapon_Assault_Rifle,
			Weapon_Shotgun_pump_action,
			Weapon_Rocket_Launcher,
			Weapon_Silenced_Pistol,
			Weapon_Revolver,
			Weapon_1H_SMG,
			Weapon_Silenced_SMG,
			Weapon_2H_SMG,
			Weapon_Sawed_off_shotgun,
			Weapon_Grenade_Launcher,
			Weapon_Disposable_RPG,
			Weapon_Sniper_rifle,
			Weapon_Assault_rifle_sniper,
			Weapon_Assault_Rifle_heavy,
			Weapon_Assault_Rifle_high_tech,
			Weapon_Shotgun_automatic,
			Weapon_Grapplinghook,
			Weapon_Timed_Explosive,
			Weapon_Triggered_Explosive,
			Weapon_Remote_Trigger,
		};

		ImGui::Text("Selected Weapon Name: %s", jc::vars::weapons_id_to_type_name.find(weapon_ids[weapon_to_give])->second.c_str());
		ImGui::SliderInt("Weapon to give##ap.weap.tgiv", &weapon_to_give, 0, sizeof(weapon_ids) - 1);

		if (ImGui::Button("Give weapon##ap.weap.giv"))
			local_char->set_weapon(weapon_ids[weapon_to_give], false);

		if (ImGui::Button("Give New Weapon :O##ap.weap.givnew"))
			local_char->set_weapon(100, false);

		if (ImGui::Button("Give Grenade :O##ap.weap.givnade"))
			local_char->set_grenades_ammo(local_char->get_grenades_ammo() + 1);

		if (ImGui::Button("Give Debug List##ap.weap.givdl"))
		{
			local_char->set_weapon(Weapon_Pistol, false);
			local_char->set_weapon(Weapon_Assault_Rifle, false);
			local_char->set_weapon(Weapon_1H_SMG, false);
			local_char->set_weapon(Weapon_Grapplinghook, false);
			local_char->set_weapon(Weapon_Timed_Explosive, false);
			local_char->set_weapon(Weapon_Triggered_Explosive, false);
			local_char->set_weapon(Weapon_Remote_Trigger, false);

			for (int i = 0; i < WeaponSlot_Max; ++i)
				local_char->get_weapon_belt()->set_weapon_ammo(i, 60);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Skins"))
	{
		static int skin_to_set = 0;
		static int head_skin = -1;
		static int cloth_skin = -1;

		static std::vector<VariantPropInfo> accessories =
		{
			{
				.prop = -1,
				.loc = 0,
			},

			{
				.prop = -1,
				.loc = 0,
			},

			{
				.prop = -1,
				.loc = 0,
			},

			{
				.prop = -1,
				.loc = 0,
			},

			{
				.prop = -1,
				.loc = 0,
			},
		};

		ImGui::SliderInt("Skin to set##ap.skn.tset", &skin_to_set, 0, 153);

		if (ImGui::Button("Set Skin##ap.skn.set"))
			local_char->set_skin(skin_to_set);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::SliderInt("Cloth Skin##ap.cskn.set", &cloth_skin, -1, 26))
			local_char->set_npc_variant(cloth_skin, head_skin, -1, accessories);

		if (ImGui::SliderInt("Head Skin##ap.hskn.set", &head_skin, -1, 8))
			local_char->set_npc_variant(cloth_skin, head_skin, -1, accessories);

		for (int i = 0; i < 5; ++i)
		{
			ImGui::Text(("Prop: " + jc::vars::npc_variants::props[accessories[i].prop]).c_str());
			ImGui::Text(("Location: " + jc::vars::npc_variants::prop_locs[accessories[i].loc]).c_str());

			bool any_changed = false;

			if (ImGui::SliderInt(("Prop##ap.prop.set" + std::to_string(i)).c_str(), &accessories[i].prop, -1, 74))
				any_changed = true;

			if (ImGui::SliderInt(("Prop Location##ap.propl.set" + std::to_string(i)).c_str(), &accessories[i].loc, 0, 19))
				any_changed = true;

			if (any_changed)
				local_char->set_npc_variant(cloth_skin, head_skin, -1, accessories);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Day Cycle"))
	{
		auto hour = g_day_cycle->get_hour();

		bool enabled = g_day_cycle->is_enabled();

		if (ImGui::Checkbox("Enabled", &enabled))
			g_day_cycle->set_enabled(enabled);

		if (ImGui::SliderFloat("Time##daycycle", &hour, 0.f, static_cast<float>(g_day_cycle->get_hours_per_day())))
			g_day_cycle->set_time(hour);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Vehicles"))
	{
		static int veh_to_spawn = 0;
		static int veh_faction = 0;

		ImGui::Text("Selected Vehicle's EE name: %s", jc::vars::exported_entities_vehicles.find(veh_to_spawn)->second.c_str());

		ImGui::SliderInt("Vehicle to spawn##ap.veh.tspw", &veh_to_spawn, 0, 109);
		ImGui::SliderInt("Vehicle faction ##ap.veh.tfac", &veh_faction, 0, 8);

		if (ImGui::Button("Spawn Vehicle##ap.veh.spw"))
		{
			TransformTR transform(g_world->get_localplayer_character()->get_position() + vec3(2.f, 1.f, 0.f));

			g_net->send(Packet(WorldPID_SpawnObject, ChannelID_World, NetObject_Vehicle, jc::vars::exported_entities_vehicles.find(veh_to_spawn)->second, transform));

			log(RED, "wants to spawn {}", veh_to_spawn);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Debug Stuff"))
	{
		ImGui::SliderFloat("Image X", &image_x, 0.f, 1.f);
		ImGui::SliderFloat("Image Y", &image_y, 0.f, 1.f);

		ImGui::TreePop();
	}

	ImGui::End();
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

		const auto bar_pos = ImVec2(sx / 2.f - download_bar.width / 2.f, sy - 100.f);

		draw_filled_rect(bar_pos.x, bar_pos.y, download_bar.width, download_bar.height, { 0.f, 0.f, 0.f, 0.5f });
		draw_filled_rect(bar_pos.x + 2.f, bar_pos.y + 2.f, progress * (download_bar.width - 4.f), download_bar.height - 4.f, { 0.222f, 0.476f, 0.351f, 1.f });

		add_text(std::format("Downloading '{}'... {:.0f} % ({:.1f} / {:.1f} MB)",
			download_bar.current_file,
			progress * 100.f,
			download_bar.smoothed_progress / (1024.f * 1024.f),
			download_bar.target / (1024.f * 1024.f)).c_str(), sx / 2.f, bar_pos.y + 13.f, 18.f, { 1.f, 1.f, 1.f, 1.f }, true, 0);
	}
}

void UI::overlay_debug()
{
	const auto camera = g_camera->get_main_camera();
	const auto v_list = ImGui::GetBackgroundDrawList();
	const auto red_color = ImColor(255, 0, 0);
	const auto green_color = ImColor(0, 255, 0);

	g_ammo->for_each_bullet([&](int i, Bullet* bullet)
	{
		if (show_bullets)
		{
			vec2 sp_root;

			if (camera->w2s(bullet->get_position(), sp_root))
				v_list->AddText({ sp_root.x, sp_root.y }, green_color, "PEW");
		}
	});

	const auto character_list = g_world->get_characters();

	if (const auto local_player_pawn = g_world->get_localplayer_character())
	{
		for (int i = 0; i < 10; ++i)
			ImGui::Spacing();

		const auto local_head = local_player_pawn->get_head_bone_position();

		const auto weapon_belt = local_player_pawn->get_weapon_belt();

		const auto local_pos = local_player_pawn->get_position();

		auto local_transform = local_player_pawn->get_transform();

		if (no_clip)
		{
			if (g_key->is_key_down(VK_F2))
			{
				constexpr auto magnitude = 1.5f;
				const auto fp = g_camera->get_main_camera()->get_model_forward_vector();
				vec3 forward_position = {};
				forward_position.x = local_pos.x + (-fp.x * magnitude);
				forward_position.y = local_pos.y + (-fp.y * magnitude);
				forward_position.z = local_pos.z + (-fp.z * magnitude);

				local_player_pawn->set_position(forward_position);
			}
		}

		if (show_top_dbg)
		{
			if (const auto weapon = weapon_belt->get_current_weapon())
			{
				if (const auto weapon_info = weapon->get_info())
				{
					ImGui::Text("------------------");
					ImGui::Text("Ptr: 0x%x", *weapon);
					ImGui::Text("ID: %i", weapon_info->get_type_id());
					//ImGui::Text("Slot: %i", weapon_belt->get_weapon_slot(*weapon));
					//ImGui::Text("Weapon from slot: 0x%x", *weapon_belt->get_weapon_from_slot(weapon_belt->get_weapon_slot(*weapon)));
				}

				if (show_grip)
				{
					if (vec2 out_sp; camera->w2s(weapon->get_grip_transform()->get_position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFFFFFF, 30, 2.f);
				}

				if (show_last_muzzle)
				{
					if (vec2 out_sp; camera->w2s(weapon->get_last_muzzle_transform()->get_position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFFFF00, 30, 2.f);
				}

				if (show_muzzle)
				{
					if (vec2 out_sp; camera->w2s(weapon->get_muzzle_transform()->get_position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFF00FF, 30, 2.f);
				}

				if (show_last_grip)
				{
					if (vec2 out_sp; camera->w2s(weapon->get_last_ejector_transform()->get_position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFF00FFFF, 30, 2.f);
				}

				/*if (vec2 out_sp; camera->w2s(local_player_pawn->get_aim_target(), out_sp)) /// aim target
					v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFF00FFFF, 30, 2.f);*/
			}

			if (const auto vehicle_seat = local_player_pawn->get_vehicle_seat())
			{
				ImGui::Text("VehicleSeat: 0x%x", *vehicle_seat);

				/*if (vec2 out_sp; camera->w2s(vehicle_seat->get_transform()->get_position(), out_sp))
					v_list->AddCircle({ out_sp.x, out_sp.y }, 20.f, 0xFFFFFFFF, 30, 5.f);*/

				if (const auto vehicle = vehicle_seat->get_vehicle())
				{
					ImGui::Text("Vehicle: 0x%x", vehicle);
				}
			}
		}

		if (g_key->is_key_pressed(KEY_X))
			if (auto veh = local_player_pawn->get_vehicle(); veh && veh->get_driver_seat()->get_character() == local_player_pawn)
			{
				veh->set_engine_state(!veh->get_engine_state());

			}

		ptr			   closest_hp_ptr = 0;
		static Weapon* closest_weapon = nullptr;
		float		   closest_ped_distance = FLT_MAX;

		for (const auto pawn : character_list)
		{
			if (pawn == local_player_pawn)
				continue;

			/*if (!pawn->is_alive())
				continue;*/

			const auto pawn_position = pawn->get_position();
			const auto pawn_head_position = pawn->get_head_bone_position();

			vec2 sp_root, sp_head;

			// Skip if behind a screen.
			//
			if (!camera->w2s(pawn_position, sp_root))
				continue;
			if (!camera->w2s(pawn_head_position, sp_head))
				continue;

			ray_hit_info hit_info;

			bool res = g_physics->raycast(local_head, pawn_head_position, hit_info);

			auto is_visible = true;

			/*if (hit_info.object)
				is_visible = true;*/

			const auto pawn_transform = pawn->get_transform();

			const float distance = glm::distance(local_player_pawn->get_position(), pawn_position);

			/*if (g_key->is_key_pressed(VK_MULTIPLY))
				pawn->set_animation(R"(Models\Characters\Animations\Special\plant_bomb.anim)", 0.1f);*/

			if (distance <= closest_ped_distance)
			{
				closest_ped_distance = distance;
				// closest_weapon = pawn->get_weapon_belt() ? pawn->get_weapon_belt()->get_current_weapon() : nullptr;
			}

			const auto root_screen_position = ImVec2(sp_root.x, sp_root.y);

			auto pad = 0;
			if (show_health)
			{
				v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 15.f }, is_visible ? green_color : red_color, std::format("{:.1f}%", pawn->get_hp()).c_str());
				pad += 1;
			}
			if (show_velocity)
			{
				const auto vel = pawn->get_velocity();
				v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 15.f }, is_visible ? green_color : red_color, std::format("{:.1f} ({:.1f}, {:.1f}, {:.1f})", glm::length(vel), vel.x, vel.y, vel.z).c_str());
				pad += 1;
			}
			if (show_distance)
			{
				v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 15.f }, is_visible ? green_color : red_color, std::format("{:.4f}", distance).c_str());
				pad += 1;
			}
			//if (show_distance)
			{
				//v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 10.f }, is_visible ? green_color : red_color, std::format("{:x}", ptr(pawn)).c_str());
				pad += 1;
			}
			/*if (true)
			{
				const auto id = pawn->get_body_stance()->get_movement_id();
				v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 15.f }, is_visible ? green_color : red_color, std::format("{}", id).c_str());
				pad += 1;
			}*/
			if (show_skeleton)
			{
				static std::vector<std::pair<BoneID, BoneID>> connections = {
					{ Head, Neck },

					{ Neck, ClavicleL },
					{ Neck, ClavicleR },
					{ Neck, Stomach },
					{ Stomach, Waist },
					{ Waist, Pelvis },
					{ Pelvis, LegL },
					{ Pelvis, LegR },

					{ ClavicleL, ShoulderArmL },
					{ ShoulderArmL, ElbowL },
					{ ElbowL, HandL },
					{ HandL, FingersL },

					{ ClavicleR, ShoulderArmR },
					{ ShoulderArmR, ElbowR },
					{ ElbowR, HandR },
					{ HandR, FingersR },

					{ LegL, KneeL },
					{ KneeL, FootL },

					{ LegR, KneeR },
					{ KneeR, FootR },
				};

				for (const auto& [from, to] : connections)
				{
					const auto bone_from = pawn->get_bone_position(from),
						bone_to = pawn->get_bone_position(to);

					vec2 bone_from_sp,
						bone_to_sp;

					camera->w2s(bone_from, bone_from_sp);
					camera->w2s(bone_to, bone_to_sp);

					v_list->AddLine(ImVec2(bone_from_sp.x, bone_from_sp.y), ImVec2(bone_to_sp.x, bone_to_sp.y), is_visible ? green_color : red_color);
				}
			}

			if (show_view_direction)
			{
				const auto fp = pawn->get_view_direction();
				vec3 view_dir_position = {};
				view_dir_position.x = pawn_head_position.x + fp.x * 1.5f;
				view_dir_position.y = pawn_head_position.y + fp.y * 1.5f;
				view_dir_position.z = pawn_head_position.z + fp.z * 1.5f;

				vec2 sp_view_dir;

				// Skip if behind a screen.
				//
				if (!camera->w2s(view_dir_position, sp_view_dir))
					continue;

				v_list->AddLine({ sp_head.x, sp_head.y }, { sp_view_dir.x, sp_view_dir.y }, is_visible ? green_color : red_color);
			}
		}

		if (show_top_dbg)
		{
			ImGui::Text("------------------");
			ImGui::Text("Closest HP ptr: 0x%x", closest_hp_ptr);
			ImGui::Text("Facing object: 0x%x", ptr(local_player_pawn->get_facing_object()));

			if (closest_weapon)
				ImGui::Text("Closest Weapon: 0x%x", ptr(closest_weapon));
			ImGui::Text("------------------");

			vec3 lt;
			quat lr;
			vec3 ls;

			{
				local_transform.decompose(lt, lr, ls);

				ImGui::Text("Position: %.2f %.2f %.2f", lt.x, lt.y, lt.z);
				ImGui::Text("Rotation: %.2f %.2f %.2f %.2f", lr.w, lr.x, lr.y, lr.z);
				ImGui::Text("Scale: %.2f %.2f %.2f", ls.x, ls.y, ls.z);
			}
		}
	}
}

void UI::net_debug()
{
	const auto stat_level = g_net->get_net_stat();

	if (stat_level == 0)
		return;

	const auto localplayer = g_net->get_localplayer();
	if (!localplayer)
		return;

	static TimerRaw kbs_timer(1000);

	if (const auto peer = g_net->get_peer())
	{
		static uint64_t kbs_recv = 0ull,
						kbs_sent = 0ull;

		static float packet_loss = 0.f;

		ImGui::SetCursorPos({ 10.f, io->DisplaySize.y / 2.f + 100.f });

		auto render_text = [](const std::string& text)
		{
			ImGui::SetCursorPos({ 10.f, ImGui::GetCursorPosY() });
			ImGui::Text(text.c_str());
		};

		if (stat_level > 0)
			render_text(FORMATV("Ping: {}", peer->roundTripTime));

		if (stat_level > 1)
		{
			render_text(FORMATV("Upload: {:.3f} KB/s", float(kbs_sent) / 1000.f).c_str());
			render_text(FORMATV("Download: {:.3f} KB/s", float(kbs_recv) / 1000.f).c_str());
		}

		if (stat_level > 2)
		{
			render_text(FORMATV("Network ID: {:x}", localplayer->get_nid()).c_str());
			render_text(FORMATV("Packet Loss: {:.2f} %%", packet_loss).c_str());
			render_text(FORMATV("Packets Lost: {}", peer->packetsLost).c_str());

			if (peer->outgoingDataTotal >= 1000.f * 1000.f)
				render_text(FORMATV("Total Uploaded: {:.3f} MB", float(peer->outgoingDataTotal) / (1000.f * 1000.f)));
			else render_text(FORMATV("Total Uploaded: {:.3f} KB", float(peer->outgoingDataTotal) / 1000.f));

			if (peer->incomingDataTotal >= 1000.f * 1000.f)
				render_text(FORMATV("Total Downloaded: {:.3f} MB", float(peer->incomingDataTotal) / (1000.f * 1000.f)));
			else render_text(FORMATV("Total Downloaded: {:.3f} KB", float(peer->incomingDataTotal) / 1000.f));

			render_text(FORMATV("Total Packets Sent: {}", peer->totalPacketsSent));
			render_text(FORMATV("Total Packets Lost: {}", peer->totalPacketsLost));
		}
		
		if (kbs_timer.ready())
		{
			kbs_recv = peer->totalDataReceived;
			kbs_sent = peer->totalDataSent;
			packet_loss = (float(peer->packetLoss) / 65535.f) * 100.f;

			peer->totalDataReceived = 0ull;
			peer->totalDataSent = 0ull;
			peer->packetLoss = 0u;
		}
	}
}

void UI::end()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}