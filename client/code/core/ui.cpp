#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"

#include <game/sys/all.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/vehicle_controller.h>
#include <game/object/transform/transform.h>
#include <game/object/camera/camera.h>
#include <game/object/weapon/bullet.h>
#include <game/object/weapon/weapon.h>
#include <game/object/weapon/weapon_belt.h>
#include <game/object/spawn_point/agent_spawn_point.h>
#include <game/object/spawn_point/vehicle_spawn_point.h>

#include <mp/chat/chat.h>

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

	// Fuck light mode.

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(jc_hwnd);
	ImGui_ImplDX9_Init(dx_device);

	g_key->set_wnd_proc(jc_hwnd);

	initialized = true;

	log(RED, "[{:x}] created UI", GetCurrentThreadId());
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
	g_key->hijack_engine_io(false);

	initialized = false;
	destroyed	= true;
	destroying	= false;
}

void UI::dispatch()
{
	// if the system is marked as being destroyed then we want to
	// destroy all resources used by DX and ImGui from the same thread it was
	// created (aka this one from the hook, the game one)

	if (is_destroying())
		destroy();
	else if (!is_destroyed())
	{
#ifdef JC_DBG
		if (g_key->is_key_pressed(VK_INSERT))
			toggle_debug();
#endif

		begin();
		render();
		end();
	}
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

void UI::wait_until_destruction()
{
	destroying = true;

	while (!destroyed)
		SwitchToThread();
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

	ImGui::GetIO().MouseDrawCursor = show_debug;
}

void UI::render()
{
	build_debug();

	const auto camera = g_camera->get_main_camera();

	static bool	   open_overlay = true;
	constexpr auto window_pos	= ImVec2(0.0f, 0.0f);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
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
		const auto v_list	   = ImGui::GetBackgroundDrawList();
		const auto red_color   = ImColor(255, 0, 0);
		const auto green_color = ImColor(0, 255, 0);

		g_chat->update();

		g_ammo->for_each_bullet([&](int i, Bullet* bullet)
			{
				if (show_bullets)
				{
					vec2 sp_root;

					if (camera->w2s(bullet->get_position(), sp_root))
						v_list->AddText({ sp_root.x, sp_root.y }, green_color, "BULLET");
				} });

		const auto character_list = g_world->get_characters();

		if (const auto local_player_pawn = g_world->get_localplayer_character())
		{
			const auto local_head = local_player_pawn->get_head_bone_position();

			const auto weapon_belt = local_player_pawn->get_weapon_belt();

			if (no_clip)
			{
				if (g_key->is_key_down(VK_F2))
				{
					auto		   root_position = local_player_pawn->get_position();
					constexpr auto magnitude	 = 3.0001f;
					const auto [x, y, z]		 = g_camera->get_main_camera()->get_model_forward_vector();
					vec3 forward_position		 = {};
					forward_position.x			 = root_position.x + (-x * magnitude);
					forward_position.y			 = root_position.y + (-y * magnitude);
					forward_position.z			 = root_position.z + (-z * magnitude);

					local_player_pawn->set_position(forward_position);
				}
			}

			if (const auto weapon = weapon_belt->get_current_weapon())
			{
				if (const auto weapon_info = weapon->get_info())
				{
					weapon_info->set_bullets_to_fire(bullets_per_shoot);
				}

				if (show_grip)
				{
					if (vec2 out_sp; camera->w2s(*weapon->get_grip_transform()->position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFFFFFF, 30, 2.f);
				}

				if (show_last_muzzle)
				{
					if (vec2 out_sp; camera->w2s(*weapon->get_last_muzzle_transform()->position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFFFF00, 30, 2.f);
				}

				if (show_muzzle)
				{
					if (vec2 out_sp; camera->w2s(*weapon->get_muzzle_transform()->position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFFFF00FF, 30, 2.f);
				}

				if (show_last_grip)
				{
					if (vec2 out_sp; camera->w2s(*weapon->get_last_ejector_transform()->position(), out_sp))
						v_list->AddCircle({ out_sp.x, out_sp.y }, 5.f, 0xFF00FFFF, 30, 2.f);
				}
			}

			if (const auto vehicle_controller = local_player_pawn->get_vehicle_controller())
			{
				ImGui::Text("VehicleController: 0x%x", vehicle_controller);

				if (vec2 out_sp; camera->w2s(*vehicle_controller->get_transform()->position(), out_sp))
					v_list->AddCircle({ out_sp.x, out_sp.y }, 20.f, 0xFFFFFFFF, 30, 5.f);

				if (const auto vehicle = vehicle_controller->get_vehicle())
				{
					ImGui::Text("Vehicle: 0x%x", vehicle);
				}
			}

			ptr			   closest_hp_ptr		= 0;
			static Weapon* closest_weapon		= nullptr;
			float		   closest_ped_distance = FLT_MAX;

			for (const auto pawn : character_list)
			{
				if (pawn == local_player_pawn)
					continue;

				/*if (!pawn->is_alive())
					continue;*/

				const auto pawn_position	  = pawn->get_position();
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

				auto is_visible = false;

				if (hit_info.object)
					is_visible = true;

				const float distance = local_player_pawn->get_position().distance(pawn_position);

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
					v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 10.f }, is_visible ? green_color : red_color, std::format("{:.1f}%", pawn->get_hp()).c_str());
					pad += 1;
				}
				if (show_velocity)
				{
					v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 10.f }, is_visible ? green_color : red_color, std::format("{:.1f}", pawn->get_velocity().length()).c_str());
					pad += 1;
				}
				if (show_distance)
				{
					v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 10.f }, is_visible ? green_color : red_color, std::format("{:.4f}", distance).c_str());
					pad += 1;
				}
				//if (show_distance)
				{
					//v_list->AddText({ root_screen_position.x, root_screen_position.y + pad * 10.f }, is_visible ? green_color : red_color, std::format("{:x}", ptr(pawn)).c_str());
					pad += 1;
				}
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
								   bone_to	 = pawn->get_bone_position(to);

						vec2 bone_from_sp,
							bone_to_sp;

						camera->w2s(bone_from, bone_from_sp);
						camera->w2s(bone_to, bone_to_sp);

						v_list->AddLine(ImVec2(bone_from_sp.x, bone_from_sp.y), ImVec2(bone_to_sp.x, bone_to_sp.y), is_visible ? green_color : red_color);
					}
				}

				if (show_view_direction)
				{
					const auto [x, y, z]   = pawn->get_view_direction();
					vec3 view_dir_position = {};
					view_dir_position.x	   = pawn_head_position.x + x * 1.5f;
					view_dir_position.y	   = pawn_head_position.y + y * 1.5f;
					view_dir_position.z	   = pawn_head_position.z + z * 1.5f;

					vec2 sp_view_dir;

					// Skip if behind a screen.
					//
					if (!camera->w2s(view_dir_position, sp_view_dir))
						continue;

					v_list->AddLine({ sp_head.x, sp_head.y }, { sp_view_dir.x, sp_view_dir.y }, is_visible ? green_color : red_color);
				}
			}

			ImGui::Text("Closest HP ptr: 0x%x", closest_hp_ptr);

			if (closest_weapon)
				ImGui::Text("Closest Weapon: 0x%x", ptr(closest_weapon));
		}
	}
	ImGui::End();
}

void UI::build_debug()
{
	g_key->hijack_engine_io(show_debug);

	if (!show_debug)
		return;

	ImGui::Begin("JC Debug");

	if (ImGui::CollapsingHeader("General"))
	{
		ImGui::Checkbox("No clip", &no_clip);
	}

	if (ImGui::CollapsingHeader("Characters"))
	{
		ImGui::Checkbox("Skeleton", &show_skeleton);
		ImGui::Checkbox("Velocity", &show_velocity);
		ImGui::Checkbox("Distance", &show_distance);
		ImGui::Checkbox("Health", &show_health);
		ImGui::Checkbox("View direction", &show_view_direction);
	}

	if (ImGui::CollapsingHeader("Spawning"))
	{
		if (ImGui::Button(spawn_vtable_hooked ? "Unhook" : "Hook"))
		{
			// Hook CAgentSpawnPoint vtable.
			//
			constexpr auto table_start = static_cast<uintptr_t>(0x00A69340);
			constexpr auto table_end   = static_cast<uintptr_t>(0x00A693EC);
			const auto	   p_table	   = reinterpret_cast<uintptr_t*>(table_start);

			const auto	size	  = (table_end - table_start) / sizeof uintptr_t;
			static auto only_once = true;
			for (auto i = 0; i < size; i++)
			{
				if (only_once)
				{
					original_functions[i] = p_table[i];
				}
				if (i == 14)
					continue;

				if (i == 16)
					continue;

				if (!spawn_vtable_hooked)
				{
					p_table[i] = reinterpret_cast<uintptr_t>(neutral_hook2);
				}
				else
				{
					p_table[i] = original_functions[i];
				}
			}
			only_once			= false;
			spawn_vtable_hooked = !spawn_vtable_hooked;
		}
		if (ImGui::Button("Dump CASPs"))
		{
			const auto			   start_address = 0x00400000u;
			const auto			   end_address	 = 0x7FFFFFFFu;
			auto				   address		 = start_address;
			std::vector<uintptr_t> spawn_points;
			while (address < end_address)
			{
				MEMORY_BASIC_INFORMATION mbi = {};
				if (VirtualQueryEx(GetCurrentProcess(), reinterpret_cast<LPCVOID>(address), &mbi, sizeof mbi))
				{
					if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY || mbi.Protect == PAGE_EXECUTE_READWRITE)
					{
						//  CVSP 0x00A60F90
						//  CASP 0x00A69340
						// .data:00A6E630 B0 71 81 00                       PDTypeAgentVirtualTable (used by NPCs).
						// .data:00A67950 D0 05 76 00                       AgentTypeVirtualTable (not a single instance ?).
						const auto region_end = address + mbi.RegionSize;
						for (; address < region_end; address += sizeof(uintptr_t*))
						{
							if (*reinterpret_cast<uintptr_t*>(address) == 0x00A69340)
							{
								spawn_points.push_back(address);
							}
						}
					}
					address += mbi.RegionSize;
				}
				else
				{
					break;
				}
			}

			if (!spawn_points.empty())
				spawn_points.erase(spawn_points.begin());

			static AgentSpawnPoint* spawn_point = nullptr;
			if (!spawn_point)
			{
				// spawn_point = SpawnPointFactory::create_agent_spawn_point(g_world->get_localplayer_character()->get_position());
			}

			for (const auto spawn_point_address : spawn_points)
			{
				// printf("CASP 0x%p:\n\t-%s\n\t-%s\n\t-0x%p\n", spawn_point_address, spawn_point->get_spawn_name(), spawn_point->get_preset_name(), (void*)spawn_point->get_agent_type());
				//printf("0x%p\n", spawn_point_address);
			}
		}
		if (ImGui::Button("Spawn agent"))
		{
			static AgentSpawnPoint* spawn_point = nullptr;
			if (!spawn_point)
			{
				spawn_point = g_spawn->create_agent_spawn_point(g_world->get_localplayer_character()->get_position());
			}
			else
			{
				spawn_point->spawn_internal();
			}
		}
		if (ImGui::Button("Spawn vehicle"))
		{
			static VehicleSpawnPoint* spawn_point = nullptr;
			if (!spawn_point)
			{
				auto spawn_point = g_spawn->create_vehicle_spawn_point(g_world->get_localplayer_character()->get_position());
			}
			else
			{
				spawn_point->spawn();
			}
		}
	}

	if (ImGui::CollapsingHeader("Weapons"))
	{
		ImGui::Checkbox("Bullets", &show_bullets);
		ImGui::Checkbox("Muzzle", &show_muzzle);
		ImGui::Checkbox("Last muzzle", &show_last_muzzle);
		ImGui::Checkbox("Grip", &show_grip);
		ImGui::Checkbox("Last grip", &show_last_grip);
		ImGui::SliderInt("Bullets per shoot", &bullets_per_shoot, 1, 100);
	}

	if (ImGui::CollapsingHeader("Day Cycle"))
	{
		auto hour = g_day_cycle->get_hour();

		bool enabled = g_day_cycle->is_enabled();

		if (ImGui::Checkbox("Enabled", &enabled))
			g_day_cycle->set_enabled(enabled);

		if (ImGui::SliderFloat("Time##daycycle", &hour, 0.f, static_cast<float>(g_day_cycle->get_hours_per_day())))
			g_day_cycle->set_time(hour);
	}

	if (ImGui::CollapsingHeader("Vehicles"))
	{
	}
	if (ImGui::Button("Load Game"))
	{
		static constexpr uintptr_t load_manager = 0x00D32F10;

		static constexpr uintptr_t load_game_address = 0x00493400;

		typedef void(__fastcall * load_game_t)(void* ecx);

		static const auto load_game = reinterpret_cast<load_game_t>(load_game_address);

		load_game(reinterpret_cast<void*>(load_manager));
	}

	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}

void UI::end()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
