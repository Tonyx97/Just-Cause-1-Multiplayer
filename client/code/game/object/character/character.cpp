#include <defs/standard.h>
#include <defs/rg_defs.h>

#include "character.h"

#include "comps/stance_controller.h"

#include "../weapon/weapon.h"
#include "../weapon/weapon_belt.h"
#include "../character_handle/character_handle.h"
#include "../vehicle/vehicle.h"
#include "../game_player/game_player.h"
#include "../physics/pfx_character.h"

#include <game/transform/transform.h>
#include <game/sys/world/world.h>
#include <game/sys/resource/physics.h>
#include <game/sys/ai/ai_core.h>
#include <game/sys/resource/resource_streamer.h>
#include <game/sys/world/player_global_info.h>
#include <game/sys/weapon/weapon_system.h>
#include <game/sys/particle/particle_system.h>

#include <havok/character_proxy.h>
#include <havok/motion_state.h>
#include <havok/simple_shape_phantom.h>

#include <mp/net.h>

#include "../vars/anims.h"
#include "../exported_entity/exported_entity.h"
#include "../game_resource/ee_resource.h"

namespace jc::character::hook
{
	DEFINE_HOOK_THISCALL(update, 0x58FC80, void, Character* character, float delta)
	{
		/*const auto position = character->get_stomach_bone_position();

		if (character->get_flags() & (1 << 31))
		{
			if (!test)
			{
				test = g_particle->spawn_fx("firesmall_static", position, {}, {}, true, false);

				log(RED, "fire: {:x}", ptr(test));
			}

			character->set_hp(character->get_real_hp() - delta * 500.f);
		}
		else test = nullptr;

		if (test)
			jc::write(position, test, 0x4 + 0x30);*/

		return update_hook(character, delta);
	}

	DEFINE_HOOK_THISCALL(set_body_stance, 0x625750, void, BodyStanceController* stance, uint32_t id)
	{
		auto can_be_ignored = [id]()
		{
			// allow the following stances to be set to remote players by the engine itself

			switch (id)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 6:
			case 9:
			case 10:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 19:
			case 22:
			case 23:
			case 24:
			case 27:	// short fall
			case 38:	// exit vehicle (left)
			case 39:	// open vehicle door (left)
			case 37:	// close vehicle door from inside (left)
			case 40:	// close vehicle door from inside (right)
			case 41:	// open vehicle door (right)
			case 42:	// exit vehicle (right)
			case 43:	// enter vehicle (left)
			case 44:	// enter vehicle (right)
			case 45:	// close vehicle door from outside (left)
			case 46:	// close vehicle door from outside (right)
			case 47:	// passenger to driver seat
			case 48:	// airplane accelerate
			case 49:	// airplane decelerate
			case 51:
			case 52:
			case 54:	// vehicle seating
			case 56:	// vehicle looking behind
			case 58:	// lift motorbike from the ground
			case 60:	// jump out of vehicle (left)
			case 61:	// enter vehicle roof
			case 62:	// enter air vehicle roof
			case 63:	// vehicle roof to driver seat
			case 65:	// vehicle roof to diff vehicle roof
			case 66:	// mounted gun
			case 74:
			case 75:
			case 77:
			case 78:
			case 79:
			case 81:
			case 82:	// jump out of vehicle (right)
			//case 84:	// crawling injury
			case 88:	// hit by physical force
				return true;
			}

			return false;
		};

		auto can_be_synced = [id]()
		{
			switch (id)
			{
			case 21:	// jump
			//case 23:	// crouch
			case 25:	// sky dive
			case 28:	// get down the ladder (bottom side)
			case 29:
			case 30:
			case 59:	// open parachute
			case 69:	// get into ladder (bottom side)
			case 71:	// get into ladder (top side)
			case 85: return true;
			}

			return false;
		};

		if (const auto local_char = g_world->get_local_character())
		{
			if (const auto character = stance->get_character(); character == local_char)
			{
				if (!can_be_ignored())
				{
					if (can_be_synced())
						g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_BodyStance, id));
					else log(GREEN, "[DBG] Localplayer body stance set to: {} from {:x}", id, RET_ADDRESS);
				}
			}
			else if (const auto player = g_net->get_player_by_character(character))
			{
				switch (id)
				{
				case 84: // crawling injury
					return;
				}
			}
#ifdef JC_DBG
			else
			{
				switch (id)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 9:
				case 10:
				case 12:
				case 13:
				case 14:
				case 54:
				case 56: break;
				default:
					log(GREEN, "[DBG] Character body stance set to: {} from {:x}", id, RET_ADDRESS);
				}
			}
#endif
		}

		set_body_stance_hook(stance, id);
	}

	DEFINE_HOOK_THISCALL(set_arms_stance, 0x744230, void, ArmsStanceController* stance, uint32_t id)
	{
		if (const auto local_char = g_world->get_local_character())
		{
			const auto character = stance->get_character();

			if (character == local_char)
			{
				/*switch (const auto ret_add = ptr(_ReturnAddress()))
				{
				default:
				{
					switch (id)
					{
					}
				}
				}*/
			}
		}

		set_arms_stance_hook(stance, id);
	}

	DEFINE_HOOK_THISCALL_S(can_be_destroyed, 0x595F10, bool, Character* character)
	{
		switch (RET_ADDRESS)
		{
		case 0x584E3F: check(false, "[{}] This was removed in the previous update, deprecated and shouldn't be called, contact the dev", CURR_FN); break;
		}

		return false;

		// todojc - remove the whole function code if the assert above doesn't get triggered in the future

		// if our local player is being checked here, it means PlayerSettings is trying to
		// get us to the game continue menu, we will return false all the time so we can implement our
		// own spawing and also we avoid the stupid menu that serves no purpose, same for remote players
		// obviously...

		if (g_net->get_player_by_character(character))
			return false;

		// we implement our own logic of this function, we can modify the amount of time
		// a character can stay death before its destruction or we can just skip it in the future
		// and so on, this will vary in future development

		const auto stance_controller = character->get_body_stance();
		const auto movement_id = stance_controller->get_movement_id();

		return character->get_max_hp() <= std::numeric_limits<float>::max() &&
			(movement_id == 0x34 || movement_id == 0x62) &&
			character->get_death_time() > 10.f;
	}

	DEFINE_HOOK_THISCALL_S(is_in_ladder, 0x597A00, bool, Character* character)
	{
		const bool ret = is_in_ladder_hook(character);

		if (ret)
		{
			switch (RET_ADDRESS)
			{
			case 0x4C7E2B: // localplayer check before Character::DispatchMovement
			{
				// update forward while we are climbing a ladder so other players
				// can visually move our player
				
				if (const auto localplayer = g_net->get_localplayer())
					if (const auto local_char = localplayer->get_character())
					{
						const auto& move_info = localplayer->get_movement_info();

						const auto local_game_player = g_world->get_local();

						localplayer->set_movement_info(move_info.angle, move_info.right, local_game_player->get_forward(), move_info.aiming);
					}

				break;
			}
			}
		}

		return ret;
	}

	DEFINE_HOOK_THISCALL_S(setup_punch, 0x5A4380, Character*, Character* character)
	{
		auto res = setup_punch_hook(character);

		if (const auto local_char = g_world->get_local_character())
			if (character == local_char && res == character)
				g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_Punch));

		return res;
	}

	DEFINE_HOOK_THISCALL_S(update_mid, 0x591AD0, void, Character* character)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
			{
				const auto was_hip_aim = localplayer->is_hip_aiming(),
						   was_full_aim = localplayer->is_full_aiming(),
						   is_hip_aim = local_char->has_flag(CharacterFlag_HipAiming),
						   is_full_aim = local_char->has_flag(CharacterFlag_FullAiming);

				const auto aim_target = local_char->get_aim_target();

				if (was_hip_aim != is_hip_aim || was_full_aim != is_full_aim)
					g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_Aiming, is_hip_aim, is_full_aim, aim_target));

				localplayer->set_aim_info(is_hip_aim, is_full_aim, aim_target);
			}

		update_mid_hook(character);
	}

	DEFINE_HOOK_THISCALL(fire_weapon, 0x59FD20, bool, Character* character, bool a2)
	{
		const auto res = fire_weapon_hook(character, a2);

		if (res)
			if (const auto localplayer = g_net->get_localplayer())
				if (const auto local_char = localplayer->get_character(); character == local_char)
					if (auto weapon = local_char->get_weapon_belt()->get_current_weapon())
						if (const auto weapon_info = weapon->get_info())
						{
							// do our own spread calculation so we can send the final muzzle and direction to the server
							// to achieve perfect sync on muzzle and direction

							const auto muzzle = weapon->get_muzzle_transform()->get_position();
							const auto aim_target = weapon->get_aim_target();

							auto direction = glm::normalize(aim_target - muzzle);

							if (const auto bullets = weapon_info->get_bullets_to_fire(); bullets == 1)
							{
								const auto is_crouching = character->is_crouching();
								const auto spread_factor = is_crouching ? Character::CROUCH_SPREAD_MODIFIER() : Character::STAND_SPREAD_MODIFIER();
								const auto spread = spread_factor * glm::radians(1.f - weapon_info->get_accuracy(false));
								const auto rotation_matrix = glm::yawPitchRoll(
									util::rand::rand_flt(-1.f, 1.f) * spread,
									util::rand::rand_flt(-1.f, 1.f) * spread,
									util::rand::rand_flt(-1.f, 1.f) * spread);

								direction = vec4(direction, 0.f) * rotation_matrix;

								const auto packed_direction = jc::math::pack_vec3(direction, 1.f);
								const auto unpacked_direction = jc::math::unpack_vec3(packed_direction, 1.f);

								localplayer->set_multiple_rand_seed(0u);
								localplayer->set_bullet_direction(muzzle, unpacked_direction);

								g_net->send(Packet(
									PlayerPID_StanceAndMovement,
									ChannelID_Generic,
									PlayerStanceID_Fire,
									weapon->get_id(),
									muzzle,
									packed_direction));
							}
							else if (bullets > 1)
							{
								// non-zero rand seed to generate the rest of the pellets

								const auto rand_seed = static_cast<uint16_t>(1u + (static_cast<uint16_t>(__rdtsc()) & 0xFFFE));

								const auto packed_direction = jc::math::pack_vec3(direction, 1.f);
								const auto unpacked_direction = jc::math::unpack_vec3(packed_direction, 1.f);

								localplayer->set_multiple_rand_seed(rand_seed);
								localplayer->set_bullet_direction(muzzle, unpacked_direction);

								g_net->send(Packet(
									PlayerPID_StanceAndMovement,
									ChannelID_Generic,
									PlayerStanceID_FireMultiple,
									rand_seed,
									weapon->get_id(),
									muzzle,
									packed_direction));
							}
						}
		
		return res;
	}

	// hook the localplayer's key dispatch to reload weapon so we know if we should send
	// the reload msg or not
	//
	DEFINE_HOOK_THISCALL_S(reload_current_weapon, 0x5A0220, void, Character* character)
	{
		if (const auto lp = g_net->get_localplayer())
			if (const auto local_char = lp->get_character())
				if (const auto weapon_belt = local_char->get_weapon_belt())
					if (auto current_weapon = weapon_belt->get_current_weapon())
					{
						const auto was_reloading = current_weapon->is_reloading();

						reload_current_weapon_hook(character);

						const auto is_reloading = current_weapon->is_reloading();

						if (is_reloading && was_reloading != is_reloading)
							g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_Reload));

						return;
					}

		reload_current_weapon_hook(character);
	}

	DEFINE_HOOK_THISCALL(force_launch, 0x5A34A0, void, Character* character, vec3* dir, float f1, float f2)
	{
		if (const auto lp = g_net->get_localplayer())
			if (const auto local_char = lp->get_character())
			{
				//log(RED, "damn, {} {} {} {} {}", dir->x, dir->y, dir->z, f1, f2);

				if (local_char == character)
					g_net->send(Packet(PlayerPID_StanceAndMovement, ChannelID_Generic, PlayerStanceID_ForceLaunch, character->get_added_velocity(), *dir, f1, f2));
				else if (g_net->get_player_by_character(character))
					return;
			}

		force_launch_hook(character, dir, f1, f2);
	}

	DEFINE_HOOK_THISCALL(character_proxy_add_velocity, 0x744B20, void, hkCharacterProxy* proxy, const vec3* velocity, vec4* rotation)
	{
		// check which parts of the engine code we want to let modify a remote player's velocity

		switch (const auto ret = RET_ADDRESS)
		{
		case 0x5A3EAB: break;	// Character::CheckForceLaunch, modifying Y velocity for a better parabolic movement
		default:
		{
			log(RED, "[{}] WARNING {}", CURR_FN, ret);

			// block the localplayer from modifying a remote player's velocity
			
			if (const auto lp = g_net->get_localplayer())
				if (const auto local_char = lp->get_character())
					if (const auto character = proxy->get_character())
						if (character != local_char && g_net->get_player_by_character(character))
							return;
		}
		}

		character_proxy_add_velocity_hook(proxy, velocity, rotation);
	}

	DEFINE_HOOK_THISCALL(set_vehicle_seat, jc::character::fn::SET_VEHICLE_SEAT, void, Character* character, shared_ptr<VehicleSeat>* new_seat)
	{
		if (const auto lp = g_net->get_localplayer())
			if (const auto local_char = lp->get_character(); local_char == character)
			{
				// if the new seat is valid then get the seat type
				// otherwise, we need to grab the previous seat type

				VehicleNetObject* new_vehicle_net = nullptr;

				uint8_t seat_type = VehicleSeat_None;
				
				if (*new_seat)
				{
					seat_type = (*new_seat)->get_type();
					new_vehicle_net = g_net->get_net_object_by_game_object((*new_seat)->get_vehicle())->cast<VehicleNetObject>();
				}
				else if (const auto curr_seat = local_char->get_weak_vehicle_seat())
					seat_type = curr_seat->get_type();

				lp->set_vehicle(seat_type, new_vehicle_net);

				g_net->send(Packet(PlayerPID_SetVehicle, ChannelID_Generic, new_vehicle_net, seat_type));
			}

		set_vehicle_seat_hook(character, new_seat);
	}

	DEFINE_INLINE_HOOK_IMPL(distance_culling_check, 0x590B90)
	{
		const auto character = ihp->read_ebp<VehicleSeat*>(0x47C);
		const auto update_bones = ihp->at_ebp<bool>(0xDD);

		// todojc - temporary ignore the limit for all characters, in the future
		// we will add custom boundaries

		*update_bones = true;
	}

	// weak_ptr::reset is used when resetting a grappled object from a character
	// so hook it, check what return addresses mess with the grappled object of a character
	// and avoid remote players from resetting the instance.
	//
	DEFINE_HOOK_THISCALL_S(invalidate_weak_ptr, jc::character::fn::INVALIDATE_WEAK_PTR, void, uint8_t* grappled_object_ptr)
	{
		// ignore the cases where the grappled object is invalidated
		
		switch (RET_ADDRESS)
		{
		case 0x4C3FA9:
		case 0x4C5E7C:
		case 0x4C674C:
		case 0x4C67C4:
		case 0x4CC48B:
		case 0x52C2B7:
		case 0x59FA76:
		case 0x5A067E:
		case 0x5A2EFA:
		{
			if (const auto lp = g_net->get_localplayer())
				if (const auto local_char = lp->get_character())
					if (const auto character = BITCAST(Character*, grappled_object_ptr - jc::character::GRAPPLED_OBJECT); local_char == character)
					{
						lp->set_grappled_object(nullptr);

						g_net->send(Packet(PlayerPID_GrapplingHookAttachDetach, ChannelID_Generic, false));
					}
					else return;

			break;
		}
		}

		invalidate_weak_ptr_hook(grappled_object_ptr);
	}

	void enable(bool apply)
	{
		update_hook.hook(apply);
		set_body_stance_hook.hook(apply);
		set_arms_stance_hook.hook(apply);
		can_be_destroyed_hook.hook(apply);
		is_in_ladder_hook.hook(apply);
		setup_punch_hook.hook(apply);
		update_mid_hook.hook(apply);
		fire_weapon_hook.hook(apply);
		reload_current_weapon_hook.hook(apply);
		force_launch_hook.hook(apply);
		character_proxy_add_velocity_hook.hook(apply);
		set_vehicle_seat_hook.hook(apply);
		distance_culling_check_hook.hook(apply);
		invalidate_weak_ptr_hook.hook(apply);
	}
}

// statics

vec3 Character::GET_GRAPPLE_HOOKED_RELATIVE_POS()
{
	return jc::read<vec3>(jc::character::g::GRAPPLE_HOOKED_RELATIVE_POS);
}

void Character::SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai)
{
	if (ai)
		jc::write(v, jc::character::g::AI_PUNCH_DAMAGE);
	else jc::write(v, jc::character::g::PLAYER_PUNCH_DAMAGE);
}

float Character::GET_GLOBAL_PUNCH_DAMAGE(bool ai)
{
	return ai ? jc::read<float>(jc::character::g::AI_PUNCH_DAMAGE) : 
				jc::read<float>(jc::character::g::PLAYER_PUNCH_DAMAGE);
}

void Character::SET_FLYING_Y_MODIFIER(float v)
{
	jc::write(v, jc::character::g::FLYING_Y_MODIFIER);
}

void Character::SET_GRAPPLE_HOOKED_RELATIVE_POS(const vec3& v)
{
	jc::write(v, jc::character::g::GRAPPLE_HOOKED_RELATIVE_POS);
}

// character

void Character::rebuild_skeleton()
{
	const auto skeleton = get_skeleton();

	jc::this_call(jc::character::fn::DESTROY_SKELETON, skeleton);
	jc::this_call(jc::character::fn::CREATE_SKELETON, skeleton);
}

void Character::init_model()
{
	// make sure we patch/skip the weapon belt recreation, because, for some reason, they added
	// that inside this character info function

	scoped_patch sp(jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT.address, { 0xE9, 0x8C, 0x0, 0x0, 0x0, 0x90, 0x90, 0x90 });

	jc::v_call<ptr>(this, jc::alive_object::vt::INITIALIZE_MODELS, get_info());
}

void Character::respawn()
{
	remove_flag(1 << 31); // dbg

	// reset general character stuff
	
	jc::this_call(jc::character::fn::RESPAWN, this, 1.f);

	// reset character's capsule dimensions

	if (const auto pfx = get_pfx())
		pfx->as<PfxCharacter>()->set_capsule_dimensions(1.8f, 0.f, 0.4f);
}

void Character::set_proxy_velocity(const vec3& v)
{
	if (auto physical = get_proxy())
		if (auto p1 = jc::read<ptr>(physical, 0xB0))
			if (auto p2 = jc::read<ptr>(p1, 0x4))
				jc::write(vec4(v.x, v.y, v.z, 0.f), p2, 0x10);
}

void Character::set_added_velocity(const vec3& v)
{
	if (auto physical = get_proxy())
	{
		vec4 rotation = get_transform()[3];

		jc::character::hook::character_proxy_add_velocity_hook(physical, &v, &rotation);
	}
}

void Character::set_grenades_ammo(int32_t v)
{
	jc::write(v, this, jc::character::GRENADES_AMMO);
}

void Character::set_animation(const std::string& name, float speed, bool unk0, bool unk1)
{
	const jc::stl::string str = name;

	jc::this_call<bool>(jc::character::fn::SET_ANIMATION, this, &str, unk0, speed, unk1);
}

void Character::set_grenade_timeout(float v)
{
	jc::write(v, this, jc::character::GRENADE_TIMEOUT);
}

void Character::set_walking_anim_set(int32_t walking_anim_id, int32_t skin_id, bool sync)
{
	auto it = jc::vars::anim_sets.find(walking_anim_id);
	if (it == jc::vars::anim_sets.end())
		return;

	auto new_info = get_info();

	new_info->character_as = it->second;

	set_skin(skin_id, false);

	if (sync)
	{
		// sync the skin id with other players

		g_net->send(Packet(PlayerPID_DynamicInfo, ChannelID_Generic, PlayerDynInfo_WalkingSetAndSkin, walking_anim_id, skin_id));
	}
}

void Character::set_skin(int32_t id, bool sync)
{
	set_skin(id, -1, -1, -1, {}, sync);
}

void Character::set_skin(int32_t id, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props, bool sync)
{
	const auto ee_name = jc::vars::get_ee(id);
	if (ee_name.empty())
		return;

	g_rsrc_streamer->request_agent_ee(ee_name, [=](ExportedEntityResource* eer, const std::string&)
	{
		if (object_base_map* map = nullptr; eer->get_exported_entity()->load_class_properties(map) && map)
		{
			// let's try to get the model name from the map
			// we will try to get the normal model first, if it fails
			// the only choice we have is to check for PD model which seems
			// to be some kind of model for special characters/agents

			jc::stl::string model;

			bool use_npc_variant = false;

			if (!(use_npc_variant = map->find_string(ObjectBase::Hash_Model, model)))
				if (!map->find_string(ObjectBase::Hash_PD_Model, model))
					return;

			// Avalanche bois forgot to recreate the skeleton when setting the character info
			// so we have to do it ourselves

			rebuild_skeleton();

			// get current info and set the new model

			auto new_info = get_info();

			new_info->model = model;
			
			// update the model etc

			init_model();

			// if we specified a new npc variant through the parameters then
			// let's use it instead of the default one

			const bool use_custom_npc_variant = (cloth_skin != -1 || head_skin != -1 || cloth_color != -1 || props.size() > 0);

			if (use_custom_npc_variant)
			{
				// npc variant through set_skin is never synced

				set_npc_variant(cloth_skin, head_skin, cloth_color, props, false);
			}
			else if (use_npc_variant)
			{
				// if we need to use npc variant then create one
				// initialize it using the agent type map
				// and set it to this character

				auto npc_variant = NPCVariant::CREATE();

				npc_variant->init_from_map(map);

				set_npc_variant(npc_variant.get());
			}

			if (sync)
			{
				// sync the skin id with other players

				g_net->send(Packet(PlayerPID_DynamicInfo, ChannelID_Generic, PlayerDynInfo_Skin, id));
			}
		}
	});
}

void Character::set_npc_variant(NPCVariant* v)
{
	jc::this_call<bool>(jc::character::fn::SET_NPC_VARIANT, this, v);
}

void Character::set_npc_variant(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props, bool sync)
{
	auto get_hashes = [](int index) -> std::tuple<uint32_t, uint32_t, uint32_t>
	{
		switch (index)
		{
		case 0: return { NPCVariant::Hash_Accessory1_Slot1, NPCVariant::Hash_Accessory1_Loc, NPCVariant::Hash_ForceAccessory1 };
		case 1: return { NPCVariant::Hash_Accessory2_Slot1, NPCVariant::Hash_Accessory2_Loc, NPCVariant::Hash_ForceAccessory2 };
		case 2: return { NPCVariant::Hash_Accessory3_Slot1, NPCVariant::Hash_Accessory3_Loc, NPCVariant::Hash_ForceAccessory3 };
		case 3: return { NPCVariant::Hash_Accessory4_Slot1, NPCVariant::Hash_Accessory4_Loc, NPCVariant::Hash_ForceAccessory4 };
		case 4: return { NPCVariant::Hash_Accessory5_Slot1, NPCVariant::Hash_Accessory5_Loc, NPCVariant::Hash_ForceAccessory5 };
		}

		return { 0u, 0u, 0u };
	};

	object_base_map map {};

	if (cloth_skin != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_ClothSkinSlot1, jc::vars::npc_variants::cloth_skins[cloth_skin]);

	if (head_skin != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_HeadSkinSlot1, jc::vars::npc_variants::head_skins[head_skin]);
	
	if (cloth_color != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_ClothColor, jc::vars::npc_variants::cloth_color[cloth_color]);

	for (int i = 0; const auto& prop : props)
	{
		const auto hashes = get_hashes(i++);

		if (prop.prop != -1)
		{
			map.insert<object_base_map::String>(std::get<0>(hashes), jc::vars::npc_variants::props[prop.prop]);
			map.insert<object_base_map::String>(std::get<1>(hashes), jc::vars::npc_variants::prop_locs[prop.loc]);
			map.insert<object_base_map::Int>(std::get<2>(hashes), 1);
		}
	}
	
	map.insert<object_base_map::Int>(0x937af6a, 1);

	auto npc_variant = NPCVariant::CREATE();

	npc_variant->init_from_map(&map);

	set_npc_variant(npc_variant.get());

	if (sync)
	{
		// sync the variant info with other players

		g_net->send(Packet(PlayerPID_DynamicInfo, ChannelID_Generic, PlayerDynInfo_NPCVariant, cloth_skin, head_skin, cloth_color, props));
	}
}

void Character::set_flag(uint32_t mask)
{
	set_flags(get_flags() | mask);
}

void Character::set_flags(uint32_t mask)
{
	jc::write(mask, this, jc::character::FLAGS);
}

void Character::remove_flag(uint32_t mask)
{
	jc::write(get_flags() & ~mask, this, jc::character::FLAGS);
}

void Character::play_idle_stance()
{
	jc::write(-1.f, this, jc::character::IDLE_STANCE_TIME);
}

void Character::set_roll_clamp_enabled(bool v)
{
	jc::write(v ? 0.f : 1.f, this, jc::character::ROLL_CLAMP);
}

void Character::set_body_stance(uint32_t id)
{
	jc::character::hook::set_body_stance_hook(get_body_stance(), id);
}

void Character::set_arms_stance(uint32_t id)
{
	jc::character::hook::set_arms_stance_hook(get_arms_stance(), id);
}

void Character::setup_punch()
{
	jc::character::hook::setup_punch_hook(this);
}

void Character::force_launch(const vec3& vel, const vec3& dir, float f1, float f2)
{
	set_added_velocity(vel);

	jc::character::hook::force_launch_hook(this, BITCAST(vec3*, &dir), f1, f2);
}

void Character::clear_weapon_belt()
{
	get_weapon_belt()->clear();
}

void Character::set_weapon(uint8_t id, bool is_remote_player)
{
	if (id == 0u)
		return hide_current_weapon();

	if (const auto weapon_belt = get_weapon_belt())
	{
		const auto curr_weapon = weapon_belt->get_current_weapon();

		// if we have it and we are already holding it then 
		// do nothing

		if (curr_weapon && curr_weapon->get_id() == id)
			return;

		const auto target_weapon_type = g_weapon->get_weapon_type(id);

		// simply draw the gun if we already have it but not
		// in hands

		for (int slot = 0; slot < WeaponBelt::MAX_SLOTS(); ++slot)
			if (auto weapon = weapon_belt->get_weapon_from_slot(slot))
			{
				if (weapon->get_id() == id)
				{
					set_draw_weapon(weapon);

					return draw_weapon_now();
				}
				else if (slot == weapon_belt->get_weapon_slot(target_weapon_type))
				{
					// clear our hands

					hide_current_weapon();

					// remove the weapon before adding the new one to avoid weapon drop and bugs
					
					weapon_belt->remove_weapon(slot);

					break;
				}
			}

		// this character does not have the weapon we want to set so
		// we create and add it to the weapon belt

		auto weapon = weapon_belt->add_weapon(id);

		// do a few adjustments to the weapon if it's owned by a remote player

		if (is_remote_player)
			if (const auto weapon_info = weapon->get_info())
				weapon_info->set_infinite_ammo(true);

		// draw the weapon

		set_draw_weapon(weapon);
		draw_weapon_now();
	}
}

void Character::set_draw_weapon(int32_t slot)
{
	jc::this_call(jc::character::fn::SET_DRAW_WEAPON, this, slot);
}

void Character::set_draw_weapon(shared_ptr<Weapon>& weapon)
{
	if (const auto weapon_belt = get_weapon_belt())
	{
		const auto slot = weapon_belt->get_weapon_slot(weapon);
		const auto is_local = this == g_world->get_local_character();

		set_draw_weapon(slot);
	}
}

void Character::hide_current_weapon()
{
	set_draw_weapon(1);
	draw_weapon_now();
}

void Character::draw_weapon_now()
{
	jc::this_call(jc::character::fn::DRAW_WEAPON_NOW, this);
}

void Character::set_aim_target(const vec3& v)
{
	jc::write(v, this, jc::character::AIM_TARGET);
}

void Character::fire_current_weapon(int32_t weapon_id, const vec3& muzzle, const vec3& aim_target)
{
	// let's make sure the weapon id we want to fire is the actual current weapon

	if (const auto weapon_belt = get_weapon_belt())
		if (auto weapon = weapon_belt->get_current_weapon())
			if (const auto weapon_info = weapon->get_info(); weapon_info && weapon_info->get_id() == weapon_id)
			{
				weapon->set_last_shot_time(jc::nums::MAXF);
				weapon->set_muzzle_position(muzzle);
				weapon->set_aim_target(aim_target);
				weapon->force_fire();
			}
}

void Character::reload_current_weapon()
{
	jc::this_call<bool>(jc::character::fn::RELOAD_CURRENT_WEAPON, this);
}

void Character::set_stance_enter_vehicle_right(bool skip_anim)
{
	jc::this_call(jc::character::fn::SET_STANCE_ENTER_VEH_RIGHT, this, skip_anim);
}

void Character::set_stance_enter_vehicle_left(bool skip_anim)
{
	jc::this_call(jc::character::fn::SET_STANCE_ENTER_VEH_LEFT, this, skip_anim);
}

void Character::set_stance_enter_vehicle_no_anim()
{
	check(false, "dont");
}

void Character::set_stance_exit_vehicle_forced()
{
	jc::this_call(jc::character::fn::SET_STANCE_EXIT_VEHICLE_FORCED, this);
}

void Character::dispatch_movement(float angle, float right, float forward, bool looking)
{
	jc::this_call(jc::character::fn::DISPATCH_MOVEMENT, this, angle, right, forward, looking);
}

void Character::crouch(bool enabled)
{
	if (enabled)
		jc::this_call(jc::character::fn::CROUCH, this);
	else jc::this_call(jc::character::fn::UNCROUCH, this);
}

void Character::set_grappled_object(shared_ptr<ObjectBase> obj)
{
	if (obj.get())
		jc::this_call(jc::character::fn::SET_GRAPPLED_OBJECT, this, weak_ptr<ObjectBase>(obj));
	else jc::character::hook::invalidate_weak_ptr_hook(REF(uint8_t*, this, jc::character::GRAPPLED_OBJECT));
}

bool Character::has_flag(uint32_t mask) const
{
	return get_flags() & mask;
}

bool Character::is_standing() const
{
	return jc::this_call<bool>(jc::character::fn::IS_STANDING, this);
}

bool Character::is_on_ground() const
{
	return get_air_time() <= 0.f;
}

bool Character::is_moving() const
{
	return jc::this_call<bool>(jc::character::fn::IS_MOVING, this);
}

bool Character::is_strafing() const
{
	return jc::this_call<bool>(jc::character::fn::IS_STRAFING, this);
}

bool Character::is_opening_any_vehicle_door() const
{
	return jc::this_call<bool>(jc::character::fn::IS_OPENING_ANY_VEHICLE_DOOR, this);
}

bool Character::is_in_vehicle() const
{
	return jc::this_call<bool>(jc::character::fn::IS_IN_VEHICLE, this);
}

bool Character::is_climbing_ladder() const
{
	return jc::this_call<bool>(jc::character::fn::IS_CLIMBING_LADDER, this);
}

bool Character::is_aiming() const
{
	return jc::this_call<bool>(jc::character::fn::IS_AIMING, this);
}

bool Character::is_swimming() const
{
	return jc::this_call<bool>(jc::character::fn::IS_SWIMMING, this);
}

bool Character::is_falling() const
{
	return jc::this_call<bool>(jc::character::fn::IS_FALLING, this);
}

bool Character::is_sky_diving() const
{
	return jc::this_call<bool>(jc::character::fn::IS_SKY_DIVING, this);
}

bool Character::is_in_parachute_state() const
{
	return jc::this_call<bool>(jc::character::fn::IS_IN_PARACHUTE_STATE, this);
}

bool Character::is_paragliding() const
{
	return jc::this_call<bool>(jc::character::fn::IS_PARAGLIDING, this);
}

bool Character::is_in_stunt_position() const
{
	return jc::this_call<bool>(jc::character::fn::IS_IN_STUNT_POSITION, this);
}

bool Character::is_diving() const
{
	return jc::this_call<bool>(jc::character::fn::IS_DIVING, this);
}

bool Character::is_crouching() const
{
	return jc::this_call<bool>(jc::character::fn::IS_CROUCHING, this);
}

bool Character::is_in_air_vehicle() const
{
	return jc::this_call<bool>(jc::character::fn::IS_IN_AIR_VEHICLE, this);
}

int32_t Character::get_grenades_ammo() const
{
	return jc::read<int32_t>(this, jc::character::GRENADES_AMMO);
}

uint32_t Character::get_flags() const
{
	return jc::read<uint32_t>(this, jc::character::FLAGS);
}

float Character::get_grenade_timeout() const
{
	return jc::read<float>(this, jc::character::GRENADE_TIMEOUT);
}

float Character::get_grenade_time() const
{
	return jc::read<float>(this, jc::character::GRENADE_TIME);
}

float Character::get_death_time() const
{
	return jc::read<float>(this, jc::character::DEATH_TIME);
}

float Character::get_roll_clamp() const
{
	return jc::read<float>(this, jc::character::ROLL_CLAMP);
}

float Character::get_air_time() const
{
	return jc::read<float>(this, jc::character::AIR_TIME);
}

weak_ptr<ObjectBase> Character::get_grappled_object() const
{
	return jc::read<weak_ptr<ObjectBase>>(this, jc::character::GRAPPLED_OBJECT);
}

CharacterController* Character::get_controller() const
{
	CharacterController* controller = nullptr;

	if (this == g_world->get_local_character())
		controller = g_player_global_info->get_local_controller();

	if (!controller)
		if (const auto handle_entry = REF(CharacterHandleEntry*, this, jc::character::HANDLE_ENTRY))
			if (const auto handle = g_ai->get_character_handle_from_entry(handle_entry))
				controller = handle->get_controller();

	check(controller, "Invalid character controller {:x}", ptr(this));

	return controller;
}

Character* Character::get_facing_object() const
{
	return jc::this_call<Character*>(jc::character::fn::GET_FACING_OBJECT, this, nullptr);
}

WeaponBelt* Character::get_weapon_belt() const
{
	return jc::read<WeaponBelt*>(this, jc::character::WEAPON_BELT);
}

Vehicle* Character::get_vehicle() const
{
	const auto seat = get_vehicle_seat();
	return seat ? seat->get_vehicle() : nullptr;
}

shared_ptr<Weapon> Character::get_current_weapon() const
{
	if (const auto belt = get_weapon_belt())
		return belt->get_current_weapon();

	return {};
}

shared_ptr<VehicleSeat> Character::get_vehicle_seat() const
{
	return *REF(shared_ptr<VehicleSeat>*, this, jc::character::VEHICLE_SEAT);
}

VehicleSeat* Character::get_weak_vehicle_seat() const
{
	return jc::read<VehicleSeat*>(this, jc::character::VEHICLE_SEAT);
}

Skeleton* Character::get_skeleton() const
{
	return REF(Skeleton*, this, jc::character::SKELETON);
}

CharacterInfo* Character::get_info() const
{
	return REF(CharacterInfo*, this, jc::character::INFO);
}

BodyStanceController* Character::get_body_stance() const
{
	return REF(BodyStanceController*, this, jc::character::BODY_STANCE_CONTROLLER);
}

ArmsStanceController* Character::get_arms_stance() const
{
	return REF(ArmsStanceController*, this, jc::character::ARMS_STANCE_CONTROLLER);
}

hkCharacterProxy* Character::get_proxy() const
{
	return jc::read<hkCharacterProxy*>(this, jc::character::PHYSICAL);
}

vec3 Character::get_velocity()
{
	return *REF(vec3*, this, jc::character::VELOCITY);
}

vec3 Character::get_added_velocity() const
{
	if (auto physical = get_proxy())
		return jc::read<vec3>(physical, 0x40);

	return {};
}

vec3 Character::get_aim_target() const
{
	return jc::read<vec3>(this, jc::character::AIM_TARGET);
}

vec3 Character::get_bone_position(BoneID index) const
{
	const auto skeleton = get_skeleton();

	if (!skeleton)
		return {};

	Transform bone_transform;

	if (!skeleton->get_bone_transform(index, bone_transform))
		return {};

	return get_transform().rotate_point(bone_transform.get_position());
}

vec3 Character::get_head_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_HEAD_BONE_POSITION, this, &out);
}

vec3 Character::get_pelvis_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_PELVIS_BONE_POSITION, this, &out);
}

vec3 Character::get_stomach_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_STOMACH_BONE_POSITION, this, &out);
}

vec3 Character::get_random_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_RANDOM_BONE_POSITION, this, &out);
}

Transform Character::get_head_bone_transform()
{
	Transform out;

	return *jc::this_call<Transform*>(jc::character::fn::GET_HEAD_BONE_TRANSFORM, this, &out);
}

Transform Character::get_pelvis_bone_transform()
{
	Transform out;

	return *jc::this_call<Transform*>(jc::character::fn::GET_PELVIS_BONE_TRANSFORM, this, &out);
}