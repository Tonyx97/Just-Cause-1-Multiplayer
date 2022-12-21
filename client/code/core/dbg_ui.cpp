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
#include <game/object/game_player/game_player.h>
#include <game/object/agent_type/npc_variant.h>
#include <game/object/vars/exported_entities.h>
#include <game/object/vehicle/vehicle.h>

#include <shared_mp/objs/all.h>

#include <resource_sys/resource_system.h>

#include <mp/chat/chat.h>
#include <mp/net.h>

float image_x = 1.f,
	  image_y = 1.f;

void DebugUI::toggle_admin_panel()
{
	if (!admin_panel_available)
		return;

	show_admin_panel = !show_admin_panel;

	g_ui->get_io()->MouseDrawCursor = show_admin_panel;

	g_key->block_input(show_admin_panel);
}

void DebugUI::render_admin_panel()
{
	if (g_chat->is_typing())
		return;

	const auto main_cam = g_camera->get_active_camera();
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

	g_ui->get_io()->MouseDrawCursor = true;

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
			TransformTR transform(g_world->get_local_character()->get_position() + vec3(2.f, 1.f, 0.f));

			g_net->send(Packet(WorldPID_SpawnObject, ChannelID_World, NetObject_Vehicle, transform, jc::vars::exported_entities_vehicles.find(veh_to_spawn)->second));

			log(RED, "wants to spawn {}", veh_to_spawn);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Current Vehicle"))
	{
		const auto lp = g_world->get_local_character();

		if (auto veh = lp->get_vehicle(); veh && veh->get_driver_seat()->get_character() == lp)
		{
			if (const auto vehicle_net = g_net->get_net_object_by_game_object(veh)->cast<VehicleNetObject>())
			{
				auto color_value = vehicle_net->get_color();

				float color[4] =
				{
					float((color_value & 0xFF)) / 255.f,
					float((color_value & 0xFF00) >> 8) / 255.f,
					float((color_value & 0xFF0000) >> 16) / 255.f,
					float((color_value & 0xFF000000) >> 24) / 255.f,
				};

				if (ImGui::ColorPicker4("Vehicle Color##cv.clr", color))
				{
					auto r = uint32_t(color[0] * 255.f);
					auto g = uint32_t(color[1] * 255.f) << 8;
					auto b = uint32_t(color[2] * 255.f) << 16;
					auto a = uint32_t(color[3] * 255.f) << 24;
					
					vehicle_net->set_color(r | g | b | a, true);
				}

				static int faction_to_set = 0;

				if (ImGui::SliderInt("Faction Paintjob##cv.fac", &faction_to_set, 0, 8))
					vehicle_net->set_faction(faction_to_set, true);
			}
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

void DebugUI::overlay_debug()
{
	const auto camera = g_camera->get_active_camera();
	const auto v_list = ImGui::GetBackgroundDrawList();
	const auto red_color = ImColor(255, 0, 0);
	const auto green_color = ImColor(0, 255, 0);

#ifdef JC_DBG
	{
		//vec3 from = vec3(1324.702f, 83.395f + 1.f, 2581.040f);
		//vec3 to = vec3(1334.615f, 83.553f + 1.f, 2572.273f);

		vec3 from = vec3(411.34f, 143.97f + 1.2f, 4037.72f);
		vec3 to = vec3(423.f, 143.97f + 1.2f, 4048.54f);

		vec2 from_sp, to_sp;

		camera->w2s(from, from_sp);
		camera->w2s(to, to_sp);

		g_ui->draw_line(from_sp, to_sp, 2.f, { 0.f, 1.f, 0.f, 1.f });

		g_ui->draw_filled_circle(from_sp, 20.f, 50, { 0.f, 1.f, 1.f, 1.f });
		g_ui->draw_filled_circle(to_sp, 20.f, 50, { 1.f, 1.f, 0.f, 1.f });

		ray_hit_info hit_info;

		auto has_hit = g_physics->raycast(from, to, hit_info, true, true);

		const auto normal_end = hit_info.hit_position + hit_info.normal;

		vec2 hit_pos_sp;
		vec2 normal_sp;

		camera->w2s(hit_info.hit_position, hit_pos_sp);
		camera->w2s(normal_end, normal_sp);

		g_ui->draw_line(hit_pos_sp, normal_sp, 2.f, { 0.f, 0.f, 1.f, 1.f });
		g_ui->draw_filled_circle(hit_pos_sp, 5.f, 50, { 1.f, 0.f, 0.f, 1.f });

		if (has_hit)
			log(RED, "{:x} {:.2f} {:.2f} {:.2f} | {:.2f}", ptr(hit_info.object), hit_info.normal.x, hit_info.normal.y, hit_info.normal.z, hit_info.distance);
	}
#endif

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

	if (const auto local_player_pawn = g_world->get_local_character())
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
				const auto fp = g_camera->get_active_camera()->get_model_forward_vector();
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

void DebugUI::render_net()
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

		ImGui::SetCursorPos({ 10.f, g_ui->get_io()->DisplaySize.y / 2.f + 100.f });

		float base_y = ImGui::GetCursorPosY();

		auto render_text = [](const std::string& text, float x = 10.f)
		{
			ImGui::SetCursorPos({ x, ImGui::GetCursorPosY() });
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

		if (stat_level > 3)
		{
			ImGui::SetCursorPosY(base_y);

#if DEBUG_PACKET
			g_net->for_each_packet_sent([&](PacketID pid, uint64_t count)
			{
				render_text(FORMATV("PacketID {}: {}", pid, count), 250.f);
			});
#endif
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

void DebugUI::render_overlay()
{
	overlay_debug();
}