#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"
#include "test_units.h"

#include <game/sys/all.h>

#include <game/transform/transform.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/character/comps/vehicle_controller.h>
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

	io->Fonts->AddFontFromMemoryTTF(font_data, font_size, 22.f, nullptr, io->Fonts->GetGlyphRangesDefault());

	g_key->set_wnd_proc(jc_hwnd);

	SendMessageW(jc_hwnd, WM_SETICON, ICON_SMALL, GET_GAME_ICON());
	SendMessageW(jc_hwnd, WM_SETICON, ICON_BIG, GET_GAME_ICON());

	initialized = true;

	log(GREEN, "Created UI in thread 0x{:x}", GetCurrentThreadId());
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
	destroyed	= true;
	destroying	= false;
}

void UI::dispatch()
{
	if (g_key->is_key_pressed(VK_F1))
		toggle_admin_panel();

	begin();
	render();
	end();
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

float UI::add_text(const char* text, float x, float y, float s, const ImVec4& color, bool center, int shadow, float wrap)
{
	auto dl = ImGui::GetWindowDrawList();

	const auto size = calc_text_size(text, s, wrap);

	const auto outline_color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f)),
			   text_color = ImGui::ColorConvertFloat4ToU32(color);

	const auto position = (center ? ImVec2 { x - size.x / 2.f, y - size.y / 2.f } : ImVec2 { x, y });

	if (shadow != -1)
	{
		float shadow_cos = std::cosf(float(shadow)),
			  shadow_sin = std::sinf(float(shadow));

		dl->AddText(nullptr, s, ImVec2(position.x + shadow_cos, position.y + shadow_sin), outline_color, text, 0, wrap);
	}

	dl->AddText(nullptr, s, ImVec2(position.x, position.y), text_color, text, 0, wrap);

	return y + size.y;
}

float UI::add_text(const wchar_t* text, float x, float y, float s, const ImVec4& color, bool center, int shadow, float wrap)
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
	std::call_once(initialize_flag, [&]() { init(); });

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::GetIO().MouseDrawCursor = show_admin_panel;
}

void UI::render()
{
	render_admin_panel();

	const auto camera = g_camera->get_main_camera();

	static bool open_overlay = true;

	ImGui::SetNextWindowPos({ 0.f, 0.f }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ static_cast<float>(camera->get_width()), static_cast<float>(camera->get_height()) });

	constexpr auto window_flags = ImGuiWindowFlags_NoBackground |
								  ImGuiWindowFlags_NoCollapse |
								  ImGuiWindowFlags_NoDecoration |
								  ImGuiWindowFlags_NoResize |
								  ImGuiWindowFlags_NoSavedSettings |
								  ImGuiWindowFlags_NoFocusOnAppearing |
								  ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowBgAlpha(0.10f);

	if (ImGui::Begin("overlay", &open_overlay, window_flags))
	{
		overlay_debug();
		render_players();
		loading_screen();
		net_debug();

		g_chat->update();
	}

	ImGui::End();
}

void UI::loading_screen()
{
	if (g_net->is_joined())
		return;

	add_text("Welcome to JC:MP! Please wait until all resources are downloaded and the game loads",
		0.1f * io->DisplaySize.x, 0.45f * io->DisplaySize.y, 24.f, { 1.f, 1.f, 1.f, 1.f }, false, true, 300.f);
}

void UI::render_players()
{
	if (!g_net->is_joined())
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

	g_net->for_each_player([&](Player* player)
	{
		if (player->is_local() || !player->is_spawned())
			return;

		const auto player_char = player->get_character();
		const auto player_pos = player->get_position();

		constexpr float MAX_DISTANCE = 1000.f;

		if (auto distance_to_player = glm::distance(local_char->get_position(), player_pos); distance_to_player > 0.f && distance_to_player < MAX_DISTANCE)
		{
			const auto head_pos = player_char->get_bone_position(BoneID::Head);

			if (vec2 out; main_cam->w2s(head_pos + vec3(0.f, 0.25f, 0.f), out))
			{
				const float name_size_adjust = 1.f - (distance_to_player / MAX_DISTANCE),
							hp_bar_size_adjust = 1.f - (distance_to_player / MAX_DISTANCE),
							hp = (player->get_hp() / player->get_max_hp()) * hp_bar_size_adjust,
							hp_bar_size_x = 100.f * hp_bar_size_adjust,
							hp_border_size = 2.f;

				g_ui->draw_filled_rect(
					out.x - hp_border_size - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
					out.y - 2.f - 2.5f * hp_bar_size_adjust,
					hp_bar_size_x + hp_border_size * 2.f,
					2.5f * hp_bar_size_adjust + hp_border_size * 2.f,
					{ 0.f, 0.f, 0.f, 1.f });

				// don't try to draw this bar if the health is 0 lmao

				if (hp > 0.f)
				{
					g_ui->draw_filled_rect(
						out.x - (hp_bar_size_x / 2.f + hp_border_size / 2.f),
						out.y - 2.5f * hp_bar_size_adjust,
						hp * 100.f,
						2.5f * hp_bar_size_adjust,
						{ hp_bar_size_adjust - hp, hp, 0.f, 1.f });
				}

				g_ui->add_text(player->get_nick().c_str(), out.x, out.y - 10.f - 18.f * name_size_adjust, 18.f * name_size_adjust, { 1.f, 1.f, 1.f, 1.f }, true);
			}
		}
	});
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

	// weapon stuff

	if (auto current_weapon = local_char->get_weapon_belt()->get_current_weapon())
	{
		current_weapon->get_info()->set_infinite_ammo(infinite_ammo);
	}

	g_key->block_input(show_admin_panel);

	if (!show_admin_panel)
		return;

	ImGui::SetNextWindowSize({ 700.f, 700.f });
	ImGui::Begin("Admin Panel");

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	if (ImGui::Button("Respawn / Revive"))
		if (local_char->get_hp() <= 0.f)
			g_world->get_localplayer()->respawn();

	if (g_key->is_key_pressed(KEY_F))
		local_char->play_idle_stance();

	if (ImGui::TreeNode("Server"))
	{
		float day_time = g_day_cycle->get_hour();
		float timescale = g_time->get_time_scale();

		if (ImGui::SliderFloat("Day Hour##ap.sv.time", &day_time, 0.f, 24.f))
			g_net->send_reliable<ChannelID_Debug>(DbgPID_SetTime, day_time);

		if (ImGui::SliderFloat("Time Scale (lol)##ap.sv.ts", &timescale, 0.01f, 5.f))
			g_net->send_reliable<ChannelID_World>(WorldPID_SetTimeScale, timescale);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("General"))
	{
		ImGui::Checkbox("No clip", &no_clip);
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
		ImGui::SliderInt("Weapon to give##ap.weap.tgiv", &weapon_to_give, 0, 20);

		if (ImGui::Button("Give weapon##ap.weap.giv"))
			local_char->set_weapon(weapon_ids[weapon_to_give], false);

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

			g_net->send_reliable<ChannelID_World>(WorldPID_SpawnObject, NetObject_Vehicle, static_cast<uint16_t>(veh_to_spawn), transform);

			log(RED, "wants to spawn {}", veh_to_spawn);
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void UI::overlay_debug()
{
	if (!show_overlay_debug)
		return;

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
					ImGui::Text("Slot: %i", weapon_belt->get_weapon_slot(*weapon));
					ImGui::Text("Weapon from slot: 0x%x", *weapon_belt->get_weapon_from_slot(weapon_belt->get_weapon_slot(*weapon)));
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

			if (const auto vehicle_controller = local_player_pawn->get_vehicle_controller())
			{
				ImGui::Text("VehicleController: 0x%x", vehicle_controller);

				if (vec2 out_sp; camera->w2s(vehicle_controller->get_transform()->get_position(), out_sp))
					v_list->AddCircle({ out_sp.x, out_sp.y }, 20.f, 0xFFFFFFFF, 30, 5.f);

				if (const auto vehicle = vehicle_controller->get_vehicle())
				{
					ImGui::Text("Vehicle: 0x%x", vehicle);
				}
			}
		}

		if (g_key->is_key_pressed(KEY_X))
			if (auto veh = local_player_pawn->get_vehicle())
				veh->set_engine_state(!veh->get_engine_state());

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
					{ BoneID::Head, BoneID::Neck },

					{ BoneID::Neck, BoneID::ClavicleL },
					{ BoneID::Neck, BoneID::ClavicleR },
					{ BoneID::Neck, BoneID::Stomach },
					{ BoneID::Stomach, BoneID::Waist },
					{ BoneID::Waist, BoneID::Pelvis },
					{ BoneID::Pelvis, BoneID::LegL },
					{ BoneID::Pelvis, BoneID::LegR },

					{ BoneID::ClavicleL, BoneID::ShoulderArmL },
					{ BoneID::ShoulderArmL, BoneID::ElbowL },
					{ BoneID::ElbowL, BoneID::HandL },
					{ BoneID::HandL, BoneID::FingersL },

					{ BoneID::ClavicleR, BoneID::ShoulderArmR },
					{ BoneID::ShoulderArmR, BoneID::ElbowR },
					{ BoneID::ElbowR, BoneID::HandR },
					{ BoneID::HandR, BoneID::FingersR },

					{ BoneID::LegL, BoneID::KneeL },
					{ BoneID::KneeL, BoneID::FootL },

					{ BoneID::LegR, BoneID::KneeR },
					{ BoneID::KneeR, BoneID::FootR },
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
	if (!g_net)
		return;

	const auto stat_level = g_net->get_net_stat();

	if (stat_level == 0)
		return;

	static TimerRaw kbs_timer(1000);

	if (const auto peer = g_net->get_peer())
	{
		static uint64_t kbs_recv = 0ull,
						kbs_sent = 0ull;

		ImGui::SetCursorPos({ 10.f, io->DisplaySize.y / 2.f + 100.f });

		if (stat_level > 0)
		{
			ImGui::Text(FORMATV("Ping: {}", peer->roundTripTime).c_str());
		}

		if (stat_level > 1)
		{
			ImGui::Text(FORMATV("Upload: {:.3f} KB/s", float(kbs_sent) / 1000.f).c_str());
			ImGui::Text(FORMATV("Download: {:.3f} KB/s", float(kbs_recv) / 1000.f).c_str());
		}

		if (stat_level > 2)
		{
			ImGui::Text(FORMATV("Network ID: {:x}", g_net->get_localplayer()->get_nid()).c_str());
			ImGui::Text(FORMATV("Packet Loss: {:.2f} %%", (float(peer->packetLoss) / 65535.f) * 100.f).c_str());
			ImGui::Text(FORMATV("Packets Lost: {}", peer->packetsLost).c_str());

			if (peer->outgoingDataTotal >= 1000.f * 1000.f)
				ImGui::Text(FORMATV("Total Uploaded: {:.3f} MB", float(peer->outgoingDataTotal) / (1000.f * 1000.f)).c_str());
			else ImGui::Text(FORMATV("Total Uploaded: {:.3f} KB", float(peer->outgoingDataTotal) / 1000.f).c_str());

			if (peer->incomingDataTotal >= 1000.f * 1000.f)
				ImGui::Text(FORMATV("Total Downloaded: {:.3f} MB", float(peer->incomingDataTotal) / (1000.f * 1000.f)).c_str());
			else ImGui::Text(FORMATV("Total Downloaded: {:.3f} KB", float(peer->incomingDataTotal) / 1000.f).c_str());

			ImGui::Text(FORMATV("Total Packets Sent: {}", peer->totalPacketsSent).c_str());
			ImGui::Text(FORMATV("Total Packets Lost: {}", peer->totalPacketsLost).c_str());
		}
		
		if (kbs_timer.ready())
		{
			kbs_recv = peer->totalDataReceived;
			kbs_sent = peer->totalDataSent;

			peer->totalDataReceived = 0ull;
			peer->totalDataSent = 0ull;
		}
	}
}

void UI::end()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}